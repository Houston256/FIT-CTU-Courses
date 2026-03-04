import argparse
import os
import random
import subprocess
from concurrent.futures import ProcessPoolExecutor, as_completed
from itertools import product
from pathlib import Path

import pandas as pd
from tqdm import tqdm

RUNS_PER_INSTANCE = 1000
MAX_FLIPS = 10_000

GSAT = ["gsat2-distr/gsat2", "-p", "0.4", "-r", "time", "-i", str(MAX_FLIPS)]
PROBSAT = [
    "probSAT/probSAT",
    "--cb",
    "2.3",
    "--runs",
    "1",
    "--maxflips",
    str(MAX_FLIPS),
]

COLUMNS = [
    "algorithm",
    "instance",
    "flips",
    "max_flips",
    "satisfied_c",
    "total_c",
]


def random_seed() -> int:
    return random.randint(1, 10**6)


def parse_args():
    parser = argparse.ArgumentParser(
        description="Run SAT solvers across CNF instances."
    )
    parser.add_argument(
        "-c",
        "--cores",
        type=int,
        default=os.cpu_count() or 1,
        help="Number of workers to use (default: number of CPU cores).",
    )
    return parser.parse_args()


def worker(args: tuple):
    command_base, conf_file, _ = args
    cmd = command_base + [str(conf_file)]

    if "probsat" in command_base[0].lower():
        cmd.append(str(random_seed()))

    r = subprocess.run(
        cmd,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
    )
    return (command_base[0], conf_file.name) + tuple(map(int, r.stderr.strip().split()))


def main() -> None:
    args = parse_args()
    num_workers = max(1, args.cores)

    root = Path(__file__).parent
    data_folder = root / "data"
    instance_folder = data_folder / "instances"

    configurations = list(instance_folder.rglob("*.cnf"))
    commands = [PROBSAT, GSAT]
    task_count = len(commands) * len(configurations) * RUNS_PER_INSTANCE
    tasks = product(commands, configurations, range(RUNS_PER_INSTANCE))

    with ProcessPoolExecutor(max_workers=num_workers) as ex:
        futures = [ex.submit(worker, task) for task in tasks]
        rows = []
        for future in tqdm(as_completed(futures), total=task_count):
            rows.append(future.result())

    pd.DataFrame.from_records(rows, columns=COLUMNS).to_parquet(
        data_folder / "data.parquet",
        engine="pyarrow",
        index=False,
    )


if __name__ == "__main__":
    main()
