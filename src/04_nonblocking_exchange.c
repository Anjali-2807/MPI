/*
 * Program 4: Asynchronous Non-Blocking Communication
 * Author: Anjali Tiwari
 * Course: Parallel and Distributed Computing
 * Instructor: Dr. Sonal Yadav
 *
 * Description:
 *   Uses MPI_Isend and MPI_Irecv to overlap communication with local CPU computation,
 *   preventing potential deadlocks during bidirectional exchange.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int partner;
    int send_val, recv_val;
    MPI_Request reqs[2];
    MPI_Status stats[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(hostname, &len);

    if (size != 2) {
        if (rank == 0) {
            printf("Error: This program must be run with 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    partner = (rank == 0) ? 1 : 0;
    send_val = (rank == 0) ? 1001 : 2002;

    printf("[Host: %-15s | Rank %d] Starting non-blocking exchange with Rank %d (sending val = %d)...\n",
           hostname, rank, partner, send_val);

    // Post non-blocking receive and send
    MPI_Irecv(&recv_val, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, &reqs[0]);
    MPI_Isend(&send_val, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, &reqs[1]);

    // Overlapping local CPU computation while message transmits
    printf("[Host: %-15s | Rank %d] Overlapping: performing local computation while transmitting...\n",
           hostname, rank);
    long long local_sum = 0;
    for (int i = 0; i < 5000000; i++) {
        local_sum += (i % 7);
    }

    // Wait for communication requests to complete
    MPI_Waitall(2, reqs, stats);

    printf("[Host: %-15s | Rank %d] Transfer complete! Successfully received value = %d from Rank %d (Local sum check = %lld)\n",
           hostname, rank, recv_val, partner, local_sum);

    MPI_Finalize();
    return 0;
}
