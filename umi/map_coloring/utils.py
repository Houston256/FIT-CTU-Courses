from collections import defaultdict

import pandas as pd


def verify_solution(solution, graph):
    """Check correct graph coloring."""
    if solution is None:
        return False
    for var in solution:
        for neighbor in graph[var]:
            if solution[var] == solution[neighbor]:
                return False
    if set(solution.keys()) != set(graph.keys()):
        return False
    return True


def load_usa_graph():
    """Load USA states graph from file."""
    # https://mathworld.wolfram.com/ContiguousUSAGraph.html
    with open("data/contiguous-usa.dat", "r") as f:
        lines = f.readlines()
    res = defaultdict(set)
    for line in lines:
        i, j = line.strip().split()
        res[i].add(j)
        res[j].add(i)
    return res


def load_county_graph():
    """https://www.census.gov/geographies/reference-files/time-series/geo/county-adjacency.html"""
    df = pd.read_csv("data/county_adjacency.txt", sep="|")
    df = df.dropna()
    return df.groupby("County GEOID")["Neighbor GEOID"].apply(set).to_dict()


def load_australia_graph():
    return {
        "WA": {"NT", "SA"},
        "NT": {"WA", "SA", "Q"},
        "SA": {"WA", "NT", "Q", "NSW", "V"},
        "Q": {"NT", "SA", "NSW"},
        "NSW": {"SA", "Q", "V"},
        "V": {"SA", "NSW"},
        "T": set(),
    }


def n_edges(graph):
    edges = set()
    for node, neighbors in graph.items():
        for neighbor in neighbors:
            edge = tuple(sorted((node, neighbor)))
            edges.add(edge)
    return len(edges)
