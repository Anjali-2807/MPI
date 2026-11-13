#!/usr/bin/env bash
# ==============================================================================
# Run MPI Programs locally with 2 processes (simulating System 1 and System 2)
# ==============================================================================

set -e

PROGRAM="${1:-bin/01_basic_exchange}"

if [ ! -f "$PROGRAM" ]; then
    echo "Compiling binaries first..."
    make
fi

echo "================================================================="
echo " Running $PROGRAM locally with 2 processes (Rank 0 and Rank 1)"
echo "================================================================="

mpirun -np 2 "$PROGRAM"

echo "================================================================="

