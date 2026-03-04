import random
import time
from typing import Literal

SELECT_T = Literal["Static", "Random", "MRV", "Degree"]
ORDERING_T = Literal["Static", "LCV"]


class CSP:
    def __init__(
        self,
        graph,
        num_colors,
        select_strategy: SELECT_T = "Static",
        ordering: ORDERING_T = "Static",
    ):
        self.graph = graph
        self.variables = sorted(graph)
        self.num_colors = num_colors
        self.reset()
        self.select_strategy = select_strategy
        self.ordering = ordering

        self.rng = random.Random(42)

    def reset(self):
        self.backtracks = 0
        self.arc_revisions = 0
        self.nodes_explored = 0
        self.constraint_checks = 0
        self.time_consistency = 0
        self.time_propagation = 0
        self.time_select_var = 0
        self.dynamic_degrees = {v: len(self.graph[v]) for v in self.variables}
        self.unassigned = set(self.variables)

    def init_domains(self):
        return {v: list(range(self.num_colors)) for v in self.variables}

    def is_consistent(self, var, value, assignment):
        t0 = time.perf_counter()
        result = True
        for neighbor in self.graph[var]:
            self.constraint_checks += 1
            if neighbor in assignment and assignment[neighbor] == value:
                result = False
                break
        self.time_consistency += (time.perf_counter() - t0) * 1000
        return result

    def select_var(self, domains):
        res = None
        t0 = time.perf_counter()
        if self.select_strategy == "Static":
            res = self.select_var_static()
        elif self.select_strategy == "Random":
            res = self.select_var_random()
        elif self.select_strategy == "MRV":
            res = self.select_var_mrv(domains)
        elif self.select_strategy == "Degree":
            res = self.select_var_degree()
        self.time_select_var += (time.perf_counter() - t0) * 1000
        return res

    def order_domain_values(self, var, domains):
        if self.ordering == "LCV":
            return self.order_domain_values_lcv(var, domains)
        return domains[var]

    def select_var_static(self):
        for v in self.variables:
            if v in self.unassigned:
                return v
        return None

    def select_var_random(self):
        """Selects a random unassigned variable."""
        return self.rng.choice(list(self.unassigned)) if self.unassigned else None

    def select_var_mrv(self, domains):
        """Minimum Remaining Values selects variable with fewest legal values in domain."""
        return min(self.unassigned, key=lambda v: len(domains[v]), default=None)

    def select_var_degree(self):
        """Degree heuristic selects variable involved in most constraints with unassigned variables."""
        return max(self.unassigned, key=lambda v: self.dynamic_degrees[v], default=None)

    def order_domain_values_lcv(self, var, domains):
        """Least Constraining Value orders values by how many choices they eliminate for neighbors."""
        values_with_conflicts = []
        for value in domains[var]:
            conflicts = sum(1 for neighbor in self.graph[var] if neighbor in self.unassigned and value in domains[neighbor])
            values_with_conflicts.append((conflicts, value))
        values_with_conflicts.sort()
        return [value for _, value in values_with_conflicts]

    def propagate(self, var, value, domains, assignment):
        return domains

    def assign(self, var, value, assignment):
        assignment[var] = value
        self.unassigned.discard(var)
        for neighbor in self.graph[var]:
            if neighbor in self.unassigned:
                self.dynamic_degrees[neighbor] -= 1

    def unassign(self, var, assignment):
        for neighbor in self.graph[var]:
            if neighbor in self.unassigned:
                self.dynamic_degrees[neighbor] += 1
        del assignment[var]
        self.unassigned.add(var)
        self.dynamic_degrees[var] = sum(1 for n in self.graph[var] if n in self.unassigned)
        self.backtracks += 1

    def solve(self):
        self.reset()
        return self._backtrack({}, self.init_domains())

    def _backtrack(self, assignment, domains):
        if len(assignment) == len(self.variables):
            return assignment

        var = self.select_var(domains)
        self.nodes_explored += 1

        for value in self.order_domain_values(var, domains):
            if self.is_consistent(var, value, assignment):
                self.assign(var, value, assignment)
                new_domains = self.propagate(var, value, domains, assignment)
                if new_domains is not None:
                    result = self._backtrack(assignment, new_domains)
                    if result is not None:
                        return result
                self.unassign(var, assignment)

        return None
