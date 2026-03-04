# MAX-SAT Simulated Annealing Solver

## Running

```bash
./eval.sh [THREADS] [EXPERIMENTS] [FILTER]
```

Results are saved to `results/`.

## Cluster Usage

Sync to cluster:
```bash
./sync.sh user@cluster:/path
```

Fetch results:
```bash
./fetch.sh user@cluster:/path
```

Or set `SYNC_DEST` in `.env`.

## Report

```bash
cd report
docker run --rm -v "$(pwd):/data" blang/latex:ubuntu pdflatex report.tex
```

## Analysis

See `analysis/README.md`.

## Data
The data can be downloaded from course pages. This is the expected structure:


```
data
└── sets
    ├── wruf36-157
    │   ├── wruf36-157-M
    │   ├── wruf36-157-M-opt.dat
    │   ├── wruf36-157-N
    │   ├── wruf36-157-N-opt.dat
    │   ├── wruf36-157-Q
    │   ├── wruf36-157-Q-opt.dat
    │   ├── wruf36-157-R
    │   └── wruf36-157-R-opt.dat
    ├── wuf20-71R
    │   ├── wuf20-71R-M
    │   ├── wuf20-71R-M-opt.dat
    │   ├── wuf20-71R-N
    │   ├── wuf20-71R-N-opt.dat
    │   ├── wuf20-71R-Q
    │   ├── wuf20-71R-Q-opt.dat
    │   ├── wuf20-71R-R
    │   └── wuf20-71R-R-opt.dat
    ├── wuf20-91
    │   ├── wuf20-91-M
    │   ├── wuf20-91-M-opt.dat
    │   ├── wuf20-91-N
    │   ├── wuf20-91-N-opt.dat
    │   ├── wuf20-91-Q
    │   ├── wuf20-91-Q-opt.dat
    │   ├── wuf20-91-R
    │   └── wuf20-91-R-opt.dat
    ├── wuf20-91R
    │   ├── wuf20-91R-M
    │   ├── wuf20-91R-M-opt.dat
    │   ├── wuf20-91R-N
    │   ├── wuf20-91R-N-opt.dat
    │   ├── wuf20-91R-Q
    │   ├── wuf20-91R-Q-opt.dat
    │   ├── wuf20-91R-R
    │   └── wuf20-91R-R-opt.dat
    ├── wuf50-218
    │   ├── wuf50-218-M
    │   ├── wuf50-218-M-opt.dat
    │   ├── wuf50-218-N
    │   ├── wuf50-218-N-opt.dat
    │   ├── wuf50-218-Q
    │   ├── wuf50-218-Q-opt.dat
    │   ├── wuf50-218-R
    │   └── wuf50-218-R-opt.dat
    └── wuf50-218R
        ├── wuf50-218R-M
        ├── wuf50-218R-M-opt.dat
        ├── wuf50-218R-N
        ├── wuf50-218R-N-opt.dat
        ├── wuf50-218R-Q
        ├── wuf50-218R-Q-opt.dat
        ├── wuf50-218R-R
        └── wuf50-218R-R-opt.dat
```