import time

from csp import CSP


class Backjumping(CSP):
    def solve(self):
        self.reset()
        result, _ = self._backjump({}, self.init_domains())
        return result

    def _backjump(self, assignment, domains):
        if len(assignment) == len(self.variables):
            return assignment, set()

        var = self.select_var(domains)
        self.nodes_explored += 1

        conflict_set = set()

        for value in self.order_domain_values(var, domains):
            local_conflicts = set()
            consistent = True

            t0 = time.perf_counter()
            for neighbor in self.graph[var]:
                self.constraint_checks += 1
                if neighbor in assignment and assignment[neighbor] == value:
                    consistent = False
                    local_conflicts.add(neighbor)
            self.time_consistency += (time.perf_counter() - t0) * 1000

            if consistent:
                self.assign(var, value, assignment)
                result, child_conflicts = self._backjump(assignment, domains)

                if result is not None:
                    return result, set()

                if var not in child_conflicts:
                    self.unassign(var, assignment)
                    return None, child_conflicts

                child_conflicts.discard(var)
                conflict_set.update(child_conflicts)
                self.unassign(var, assignment)
            else:
                conflict_set.update(local_conflicts)

        return None, conflict_set
