#!/bin/bash
# Run an MPI program across System 1 and System 2

PROG="${1:-bin/01_basic_exchange}"

if [ ! -f "$PROG" ]; then
    make
fi

echo "Running $PROG across 2 systems using hostfile..."
mpirun --hostfile hostfile -np 2 "$PROG"
