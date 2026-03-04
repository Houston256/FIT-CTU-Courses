# Academic Website Crawler
The web crawler is designed to find some path between two academic websites by strictly following hyperlinks found on each page.

## Approach
I implemented a parallelized greedy best-first search algorithm. It starts from the source URL, gets outgoing links, scores them, adds to queue.
Elements are then popped from queue and further explored in parallel. 
Because of the large branching factor of the web, I implemented **queue pruning**.
The **heuristic** is based on URL similarity to the target URL.
### Limits
To avoid getting rate limited or IP blocked, the crawler randomly waits between requests, rotates user agents, limits number of concurrent requests, and doest at most 1 request per site.
The limits are strict, but without them the crawler often gets blocked.

### Usage (uv, recommended)
```shell
uv run main.py <source-domain> <target-domain>
```
### Usage (venv)
```shell
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python main.py <source-domain> <target-domain>
```
## Example runs
Time can vary greatly, but the relative difficulty of the examples should be similar. From my tests, the fastest crawls are between czech domains and from less popular sites to more popular ones.
So FIT -> MIT is faster than MIT -> FIT.
### FIT -> MIT
```shell
uv run main.py fit.cvut.cz mit.edu 
```
```text
Crawling: https://fit.cvut.cz -> https://mit.edu


Path (9 steps):
1. https://fit.cvut.cz
2. https://www.cvut.cz/ochrana-osobnich-udaju-gdpr
3. https://international.cvut.cz/euroteq
4. https://euroteq.eurotech-universities.eu
5. https://www.polytechnique.edu/en
6. https://programmes.polytechnique.edu/en/bachelor/bachelor-of-science
7. http://web.mit.edu
8. http://learn.mit.edu
9. https://mit.edu

Time: 8.90s
```
### FIT -> VSE
```shell
uv run main.py fit.cvut.cz vse.cz
```
```text
Crawling: https://fit.cvut.cz -> https://vse.cz


Path (4 steps):
  1. https://fit.cvut.cz
  2. https://fit.cvut.cz/cs/studium/pruvodce-studiem/bakalarske-a-magisterske-studium
  3. https://www.ceskahlava.cz
  4. https://www.vse.cz

Time: 5.45s
```
### VSE -> MIT
```shell
uv run main.py vse.cz mit.edu
```

```text
Crawling: https://vse.cz -> https://mit.edu


Path (10 steps):
  1. https://vse.cz
  2. https://veda.vse.cz
  3. https://ris4seb.vse.cz
  4. https://en.ktu.edu
  5. https://seb.ktu.edu
  6. https://seb.ktu.edu/research
  7. https://sloanreview.mit.edu/article/the-new-rules-of-doing-business-with-china
  8. http://web.mit.edu
  9. http://learn.mit.edu
  10. https://mit.edu

Time: 20.20s
```

### MIT -> FIT
```text
Crawling: https://mit.edu -> https://cvut.cz


Path (23 steps):
  1. https://mit.edu
  2. http://news.mit.edu/2025/concrete-battery-now-packs-ten-times-power-1001
  3. https://libraries.mit.edu/scholarly/publishing/find-oa-articles
  4. https://bsky.app/profile/mitlibscholarly.bsky.social
  5. https://bsky.social
  6. https://en.wikipedia.org/wiki/Benefit_corporation
  7. https://it.wikipedia.org/wiki/Benefit_corporation
  8. https://it.wikipedia.org/wiki/Profitto
  9. https://eo.wikipedia.org/wiki/Profito
  10. https://eo.wikiquote.org/wiki/Profito
  11. https://de.wikiquote.org/wiki/Profit
  12. https://de.wiktionary.org/wiki/Profit
  13. https://de.wiktionary.org/wiki/profit
  14. https://nl.wiktionary.org/wiki/profit
  15. https://prirucka.ujc.cas.cz/?slovo=profit
  16. https://www.msmt.cz
  17. https://www.nauvs.cz
  18. https://radavs.cz/RVS-1.html
  19. https://www.skrvs.cz/o-nas/clenove
  20. https://www.cvut.cz
  21. https://www.cvut.cz/unikatni-vedecka-pracoviste
  22. https://netmon.fit.cvut.cz
  23. https://fit.cvut.cz

Time: 75.22s
```