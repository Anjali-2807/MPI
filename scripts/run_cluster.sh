#!/usr/bin/env bash
# ==============================================================================
# Run MPI Programs across System 1 and System 2 via Hostfile
# ==============================================================================

set -e

HOSTFILE="hostfile"
PROGRAM="${1:-bin/01_basic_exchange}"

if [ ! -f "$HOSTFILE" ]; then
    echo "Error: Hostfile '$HOSTFILE' not found!"
    exit 1
fi

if [ ! -f "$PROGRAM" ]; then
    echo "Building program..."
    make
fi

echo "================================================================="
echo " Executing $PROGRAM across System 1 and System 2"
echo " Hostfile configuration:"
cat "$HOSTFILE"
echo "================================================================="

# Run MPI across both systems (1 process on System 1, 1 process on System 2)
# If using specific network interfaces (e.g., eth0 or en0), pass --mca btl_tcp_if_include
mpirun --hostfile "$HOSTFILE" -np 2 "$PROGRAM"

echo "================================================================="
echo " Execution finished successfully."
echo "================================================================="

