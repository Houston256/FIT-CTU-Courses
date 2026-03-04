# KOP Homework 01

## Prerequisites
- Python 3.13
- gcc or clang

Use `-c/--cores` to choose how many workers run the solvers. By default it uses the number of CPU cores reported by the system.

## Option A: standard venv + pip
```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

make
python main.py #-c <number of cores>
```

## Option B (preferred): uv
```bash
uv sync --locked

make
uv run python main.py #-c <number of cores>
```
Output: `data/data.parquet` with flip counts and success flags.


## Regenerate tables and plots
```bash
uv run marimo edit .
```

## Rebuild the report (optional)
```
pdflatex report.tex
```
