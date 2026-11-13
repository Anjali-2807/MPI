#!/usr/bin/env python3
"""
Python alternative implementation using mpi4py
File: src/mpi_exchange.py
Usage: mpirun --hostfile hostfile -np 2 python3 src/mpi_exchange.py
"""

from mpi4py import MPI
import time

def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()
    processor_name = MPI.Get_processor_name()

    if size != 2:
        if rank == 0:
            print("Error: Requires exactly 2 processes.")
        return

    if rank == 0:
        # System 1
        print("=" * 65)
        print(f"[SYSTEM 1] Host: '{processor_name}' (Rank: {rank})")
        print("=" * 65)
        data = {'message': 'Hello System 2 from System 1', 'numbers': [1, 2, 3, 4, 5], 'timestamp': time.time()}
        print(f"[SYSTEM 1 -> SYSTEM 2] Dispatching Python object: {data}")
        comm.send(data, dest=1, tag=11)

        ack = comm.recv(source=1, tag=22)
        print(f"[SYSTEM 1 <- SYSTEM 2] Received response: {ack}")
        print("[SYSTEM 1] Communication completed.")
        print("=" * 65)
    elif rank == 1:
        # System 2
        print("-" * 65)
        print(f"[SYSTEM 2] Host: '{processor_name}' (Rank: {rank})")
        print("-" * 65)
        received_data = comm.recv(source=0, tag=11)
        print(f"[SYSTEM 2 <- SYSTEM 1] Received Python object: {received_data}")

        # Compute summary
        ack = {
            'status': 'SUCCESS',
            'responder': processor_name,
            'sum_computed': sum(received_data['numbers']),
            'ack_time': time.time()
        }
        print(f"[SYSTEM 2 -> SYSTEM 1] Sending reply: {ack}")
        comm.send(ack, dest=0, tag=22)
        print("[SYSTEM 2] Finished transaction.")
        print("-" * 65)

if __name__ == '__main__':
    main()

