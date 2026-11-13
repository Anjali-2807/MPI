#!/bin/bash
# Run MPI program locally with 2 processes

PROG="${1:-bin/01_basic_exchange}"

if [ ! -f "$PROG" ]; then
    make
fi

mpirun -np 2 "$PROG"
