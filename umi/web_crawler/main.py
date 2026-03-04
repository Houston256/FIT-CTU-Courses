import argparse
import asyncio
import random
import socket
import time
import warnings
from collections import defaultdict
from heapq import heapify, heappop, heappush
from urllib.parse import urljoin, urlparse

import aiohttp
from bs4 import BeautifulSoup, XMLParsedAsHTMLWarning
from fake_useragent import UserAgent
from rich.console import Console

warnings.filterwarnings("ignore", category=XMLParsedAsHTMLWarning)

BLOCKED = {
    "twitter.com",
    "facebook.com",
    "instagram.com",
    "linkedin.com",
    "youtube.com",
    "reddit.com",
    "tiktok.com",
}
SKIP_EXT = {".jpg", ".jpeg", ".png", ".gif", ".pdf", ".zip", ".svg", ".ico", ".mp4"}
KEYWORDS = {"university", "college", "education", "academic", "research", "faculty"}


def pop_k(heap, k):
    """Pop top k elements from heap"""
    return [heappop(heap) for _ in range(min(len(heap), k))]


class Crawler:
    def __init__(
        self,
        start_url,
        end_url,
        max_pages=50_000,
        concurrency=200,
        max_per_domain=100,
        verbose=False,
    ):
        """
        Crawler to find path between two URLs
        :param start_url: where to start crawling
        :param end_url: where to end crawling
        :param max_pages: limit on number of pages to visit
        :param concurrency: how many concurrent requests
        :param max_per_domain: limits number of visited pages per domain
        :param verbose: if true, print each fetched link
        """
        self.start_url = start_url.rstrip("/")
        self.end_url = end_url.rstrip("/")
        self.max_pages = max_pages
        self.concurrency = concurrency
        self.max_per_domain = max_per_domain
        self.verbose = verbose

        self.target_host = self._host(end_url)
        self.target_base = self._base_domain(end_url)
        self.target_tld = "." + self.target_base.split(".")[-1]  # .cz / .edu
        self.target_norm = self._norm(end_url)
        # target parts for keyword matching (fit.cvut.cz -> fit, cvut)
        self.target_parts = set(self.target_host.split(".")[:-1])

        # visited: normalized -> (original_url, parent_norm)
        self.visited: dict[str, tuple[str, str | None]] = {
            self._norm(start_url): (start_url, None)
        }
        self.queue: list[tuple[int, str]] = [(0, start_url)]
        self.domain_hits = defaultdict(int)
        self.console = Console()
        self.ua = UserAgent()

    def _host(self, url):
        """
        Return hostname of url without leading www.

        Example: https://www.foo.example.com/path -> foo.example.com
        """
        return urlparse(url).netloc.lower().removeprefix("www.")

    def _norm(self, url):
        """
        Normalize URL for comparison.

        Example: https://ww.foo.example.com/path -> foo.example.com
        """
        p = urlparse(url)
        path = p.path.rstrip("/") or "/"
        return f"{self._host(url)}{path}{'?' + p.query if p.query else ''}"

    def _base_domain(self, url):
        """
        Get base domain of url.

        Example: fit.cvut.cz -> cvut.cz
        """
        parts = self._host(url).split(".")
        return ".".join(parts[-2:]) if len(parts) >= 2 else parts[0]

    def _on_target_domain(self, url):
        """
        Check if URL is on the target's base domain

        Example: target = a.b.cvut.cz, url = d.cvut.cz -> True cause base is cvut.cz
        """
        return self._base_domain(url) == self.target_base

    def _subdomain_distance(self, host):
        """
        Count extra subdomains in host beyond common suffix with target.

        Example:
            target = bar.cvut.cz
            dist(cvut.cz) -> 1
            dist(foo.bar.cvut.cz) -> 2
            dist(example.com) -> 2

        host=x.y.cvut.cz -> common=cvut.cz, extra=2 (x,y)
        """
        url_parts = host.split(".")
        target_parts = self.target_host.split(".")
        common = 0
        for u, t in zip(reversed(url_parts), reversed(target_parts)):
            if u == t:
                common += 1
            else:
                break
        return len(url_parts) - common

    def _score(self, url, source):
        """
        Score url for priority queue
        Lower score = higher priority
        :param url: link to score
        :param source: link to page where url was found
        :return: score of the url
        """
        norm = self._norm(url)
        host = self._host(url)

        # exact url match, return arbitrarily high score so that the url is on top of the queue
        if norm == self.target_norm:
            return -1_000_000

        query_penalty = 100 if "?" in url else 0
        path_penalty = urlparse(url).path.count("/") * 50

        # same host (fit.cvut.cz === www.fit.cvut.cz/something)
        # penalize path depth and query presence
        if host == self.target_host:
            return -5000 + path_penalty + query_penalty

        # same base domain (fit.cvut.cz === cvut.cz) with penalized subdomain distance and path depth
        if self._on_target_domain(url):
            extra = self._subdomain_distance(host)
            return -1000 + path_penalty + query_penalty + (extra * 300)

        ##### No target match #####
        score = 0

        # parts of target in url (cvut or fit in url for fit.cvut.cz)
        if any(part in url.lower() for part in self.target_parts):
            score -= 200

        # check academic keywords
        if any(k in url.lower() for k in KEYWORDS):
            score -= 20

        # same tld (.cz/.edu/.com)
        if host.endswith(self.target_tld):
            score -= 50

        # promote exploration
        if self._base_domain(url) != self._base_domain(source):
            score -= 30

        # penalize highly visited domains
        score += self.domain_hits.get(host, 0) * 5

        return score

    def _make_url(self, base, href):
        """Convert href to absolute URL or None if invalid"""
        href = href.strip()
        if not href or href.startswith(("#", "javascript:", "mailto:", "tel:")):
            return None

        try:
            url = urljoin(base, href).split("#")[0].rstrip("/")
            p = urlparse(url)
        except ValueError:
            return None  # bad url

        if p.scheme not in ("http", "https") or not p.netloc:
            return None
        if any(b in p.netloc.lower() for b in BLOCKED):
            return None
        if any(url.lower().endswith(e) for e in SKIP_EXT):
            return None

        # reject deep paths
        if p.path.count("/") > 10:
            return None

        return url

    async def _get_outgoing_links(self, session, url):
        """Return list of outgoing links from url or empty list on error"""
        try:
            await asyncio.sleep(random.uniform(1, 3))
            headers = {"User-Agent": self.ua.random}
            async with session.get(
                url, timeout=aiohttp.ClientTimeout(total=10), headers=headers
            ) as r:
                r.raise_for_status()
                html = await r.text(errors="replace")
                soup = BeautifulSoup(html, "lxml")
                # get all links from the html page
                res = [
                    self._make_url(url, a["href"])
                    for a in soup.find_all("a", href=True)
                ]
                # drop invalid links (None)
                res = list(filter(None, res))
                if self.verbose:
                    self.console.print(url, style="green")
                return res
        except asyncio.CancelledError:
            raise
        except aiohttp.ClientResponseError as e:
            if self.verbose and e.status != 404:
                self.console.print(f"{url} ({e.status} {e.message})", style="red")
            return []
        except asyncio.TimeoutError:
            if self.verbose:
                self.console.print(f"{url} (timeout)", style="red")
            return []
        except aiohttp.ClientError as e:
            if self.verbose:
                self.console.print(f"{url} ({type(e).__name__}: {e})", style="red")
            return []

    def _prune(self, threshold=10_000):
        """
        Prune queue, keep all target candidates and a selection of others
        :param threshold: queue size which triggers pruning
        """
        if len(self.queue) < threshold:
            return

        target, other = [], []
        while self.queue:
            item = heappop(self.queue)
            (target if self._on_target_domain(item[1]) else other).append(item)

        keep_total = threshold // 10
        n_top = keep_total // 2
        n_random = keep_total - n_top

        heapify(other)
        top_links = pop_k(other, n_top)
        random_links = random.sample(other, min(n_random, len(other))) if other else []

        self.queue = target + top_links + random_links
        heapify(self.queue)
        if self.verbose:
            self.console.print(
                f"[Pruned: {len(target)} target + {len(top_links) + len(random_links)} others]",
                style="blue",
            )

    def _queue_links(self, links, source_url):
        """
        Queue outgoing links, return found target link or None
        :param links: list of links to queue
        :param source_url: url where links were found
        :return: target link if found (early stop), else None
        """
        source_norm = self._norm(source_url)

        for link in links:
            norm = self._norm(link)

            # found target, stop crawling
            if norm == self.target_norm:
                self.visited[norm] = (link, source_norm)
                return link

            if norm in self.visited:
                continue

            host = self._host(link)
            # limit per-domain visits if not on the target domain
            # the target domain is allowed to be fully crawled
            if self.domain_hits[
                host
            ] >= self.max_per_domain and not self._on_target_domain(link):
                continue

            self.visited[norm] = (link, source_norm)
            self.domain_hits[host] += 1
            score = self._score(link, source_url)
            if self.verbose and self._on_target_domain(link):
                self.console.print(
                    f"TARGET FOUND: {link} (score={score})",
                    style="yellow",
                )
            heappush(self.queue, (score, link))

        return None

    def _path(self, end_url):
        """Reconstruct url path from start to end"""
        path = []
        norm = self._norm(end_url)
        while norm:
            url, parent = self.visited.get(norm, (None, None))
            if url:
                path.append(url)
            norm = parent
        return path[::-1]

    async def crawl(self):
        """Crawl from start_url to end_url, return path or None if not found"""
        self.console.print(f"Crawling: {self.start_url} -> {self.end_url}")

        conn = aiohttp.TCPConnector(
            limit=self.concurrency,
            limit_per_host=1,
            ssl=False,
            force_close=True,
            enable_cleanup_closed=True,
            family=socket.AF_INET,
            ttl_dns_cache=300,
        )

        async with aiohttp.ClientSession(connector=conn) as session:
            pending = set()
            try:
                while (self.queue or pending) and len(self.visited) < self.max_pages:
                    self._prune()

                    while self.queue and len(pending) < self.concurrency:
                        _, url = heappop(self.queue)
                        pending.add(
                            asyncio.create_task(
                                self._get_outgoing_links(session, url), name=url
                            )
                        )

                    if not pending:
                        break

                    done, pending = await asyncio.wait(
                        pending, return_when=asyncio.FIRST_COMPLETED
                    )

                    for t in done:
                        try:
                            links = t.result()
                        except Exception:
                            links = []

                        if found := self._queue_links(links, t.get_name()):
                            return self._path(found)

                self.console.print(
                    f"No path found ({len(self.visited)} pages)", style="red"
                )
                return None

            finally:
                for t in pending:
                    t.cancel()
                await asyncio.gather(*pending, return_exceptions=True)


async def main(start_url, end_url, verbose=False):
    crawler = Crawler(
        start_url=start_url,
        end_url=end_url,
        max_pages=100_000,
        # a higher concurrency number helps with exploration / breadth.
        # don't go above ~400 though, my IP is already banned from some sites :)
        concurrency=200,
        max_per_domain=100,
        verbose=verbose,
    )

    start_time = time.time()
    path = await crawler.crawl()
    elapsed = time.time() - start_time

    if path:
        crawler.console.print(f"Path ({len(path)} steps):", style="bold")
        for i, url in enumerate(path, 1):
            crawler.console.print(f"  {i}. {url}")

    crawler.console.print(f"Time: {elapsed:.2f}s", style="bold")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Web crawler to find path between two URLs"
    )
    parser.add_argument("start", help="Starting URL")
    parser.add_argument("end", help="Target URL")
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Print individual link fetches"
    )
    args = parser.parse_args()

    start = (
        args.start
        if args.start.startswith(("http://", "https://"))
        else f"https://{args.start}"
    )
    end = (
        args.end
        if args.end.startswith(("http://", "https://"))
        else f"https://{args.end}"
    )
    asyncio.run(main(start, end, verbose=args.verbose))
