#!/bin/bash

# check if GNU parallel is installed
if ! command -v parallel &> /dev/null; then
    echo "Error: GNU parallel is not installed."
    echo "See: https://www.gnu.org/software/parallel/"
    exit 1
fi

# get arguments
THREADS=${1:-8}
EXPERIMENTS=${2:-100}
FILTER=${3:-"."}

echo "Compiling"

cmake -B build
cmake --build build

echo "Running $EXPERIMENTS experiments per instance on $THREADS threads (filter: $FILTER)..."

# clean directory
rm -fr results/

# run on multiple threads
find data -type f -name '*.mwcnf' -regex ".*\($FILTER\).*" -print0 |
  parallel -0 -j"$THREADS" --bar \
    ./build/main {} "$EXPERIMENTS"

echo "Experiments done."
