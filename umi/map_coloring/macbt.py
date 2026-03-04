import time

from csp import CSP


class MACBT(CSP):
    def solve(self):
        self.reset()
        domains = self.init_domains()
        if not self._ac3(domains):
            return None
        return self._backtrack({}, domains)

    def propagate(self, var, value, domains, assignment):
        t0 = time.perf_counter()
        new_domains = {v: list(d) for v, d in domains.items()}
        new_domains[var] = [value]
        arcs = {(n, var) for n in self.graph[var] if n not in assignment}
        result = new_domains if self._ac3(new_domains, arcs) else None
        self.time_propagation += (time.perf_counter() - t0) * 1000
        return result

    def _revise(self, xi, xj, domains):
        self.arc_revisions += 1
        to_remove = set()
        for vi in domains[xi]:
            has_support = False
            for vj in domains[xj]:
                self.constraint_checks += 1
                if vi != vj:
                    has_support = True
                    break
            if not has_support:
                to_remove.add(vi)
        domains[xi] = [v for v in domains[xi] if v not in to_remove]
        return len(to_remove) > 0

    def _ac3(self, domains, arcs=None):
        if arcs is None:
            arcs = {(xi, xj) for xi in self.graph for xj in self.graph[xi]}

        queue = set(arcs)
        while queue:
            xi, xj = queue.pop()
            if self._revise(xi, xj, domains):
                if not domains[xi]:
                    return False
                queue.update((xk, xi) for xk in self.graph[xi] if xk != xj)
        return True
