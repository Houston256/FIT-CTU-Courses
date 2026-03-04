import sys
import time

from rich.console import Console
from rich.table import Table

from bj import Backjumping
from csp import CSP
from macbt import MACBT
from utils import load_usa_graph, load_county_graph, n_edges, verify_solution, load_australia_graph

import random

random.seed(42)

sys.setrecursionlimit(10000)

table_cols = ["Nodes", "Backtracks", "Constraints", "Arc Revs", "Select (ms)", "Consist (ms)", "Propag (ms)", "Total (ms)", "Valid"]


def run_comparison():
    us = load_usa_graph()
    county = load_county_graph()
    au = load_australia_graph()
    for graph_name, graph, num_colors in [
        ("Australia state borders", au, 3),
        ("USA states connected by road", us, 4),
        ("US county borders", county, 5),
    ]:
        algorithms = [
            ("Backtracking", CSP(graph, num_colors)),
            ("Backjumping", Backjumping(graph, num_colors)),
            ("MAC-BT Static", MACBT(graph, num_colors)),
            ("MAC-BT Random", MACBT(graph, num_colors, select_strategy="Random")),
            ("MAC-BT MRV", MACBT(graph, num_colors, select_strategy="MRV")),
            ("MAC-BT MRV+LCV", MACBT(graph, num_colors, select_strategy="MRV", ordering="LCV")),
            ("MAC-BT Degree", MACBT(graph, num_colors, select_strategy="Degree")),
            ("MAC-BT Degree+LCV", MACBT(graph, num_colors, select_strategy="Degree", ordering="LCV")),
            ("MAC-BT Static+LCV", MACBT(graph, num_colors, ordering="LCV")),
        ]

        num_edges = n_edges(graph)

        table = Table(title=f"{graph_name}\n{len(graph)} nodes | {num_edges} edges | {num_colors} colors")
        table.add_column("Algorithm")
        for col in table_cols:
            table.add_column(col, justify="right")
        skip = ["backtracking", "degree", "static", "random"]
        for name, solver in algorithms:
            if any(s in name.lower() for s in skip) and len(graph) > 1000:
                continue  # wouldn't finish in reasonable time

            t0 = time.perf_counter()
            solution = solver.solve()
            total_ms = (time.perf_counter() - t0) * 1000
            valid = verify_solution(solution, graph)
            valid = "[green]Yes[/green]" if valid else "[red]No[/red]"
            table.add_row(
                name,
                f"{solver.nodes_explored:,}",
                f"{solver.backtracks:,}",
                f"{solver.constraint_checks:,}",
                f"{solver.arc_revisions:,}",
                f"{solver.time_select_var:,.3f}",
                f"{solver.time_consistency:,.3f}",
                f"{solver.time_propagation:,.3f}",
                f"{total_ms:,.3f}",
                valid,
            )

        Console().print(table)


if __name__ == "__main__":
    run_comparison()
