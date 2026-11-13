/**
 * ============================================================================
 * Program: 04_nonblocking_exchange.c
 * Purpose: Asynchronous Non-Blocking Data Exchange between Two Systems
 * Course:  Distributed & Parallel Systems
 * Author:  Parallel Computing Lab Submission
 * 
 * Description:
 *   Demonstrates non-blocking communication using MPI_Isend, MPI_Irecv, and MPI_Wait.
 *   Both System 1 and System 2 simultaneously post receive requests, then dispatch
 *   send requests. While the network transfers data in the background, each system
 *   can perform local computation, completely eliminating deadlocks.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define TAG_ASYNC 500

int main(int argc, char** argv) {
    int rank, size;
    int processor_name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int partner_rank;
    int send_val, recv_val;
    MPI_Request reqs[2];
    MPI_Status stats[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &processor_name_len);

    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Error: Exactly 2 processes required.\n");
        }
        MPI_Finalize();
        return 1;
    }

    partner_rank = (rank == 0) ? 1 : 0;
    send_val = (rank == 0) ? 1001 : 2002;

    printf("[Host: %-15s | Rank %d] Starting non-blocking exchange with Rank %d (sending val = %d)...\n",
           processor_name, rank, partner_rank, send_val);

    // 1. Post non-blocking receive first to guarantee buffer readiness
    MPI_Irecv(&recv_val, 1, MPI_INT, partner_rank, TAG_ASYNC, MPI_COMM_WORLD, &reqs[0]);

    // 2. Post non-blocking send
    MPI_Isend(&send_val, 1, MPI_INT, partner_rank, TAG_ASYNC, MPI_COMM_WORLD, &reqs[1]);

    // 3. Do simulated local computation while network transfer progresses
    printf("[Host: %-15s | Rank %d] Overlapping: performing local computation while transmitting...\n",
           processor_name, rank);
    long long local_sum = 0;
    for (int i = 0; i < 5000000; i++) {
        local_sum += (i % 7);
    }

    // 4. Wait for both communication requests to finish
    MPI_Waitall(2, reqs, stats);

    printf("[Host: %-15s | Rank %d] Transfer complete! Successfully received value = %d from Rank %d (Local sum check = %lld)\n",
           processor_name, rank, recv_val, partner_rank, local_sum);

    MPI_Finalize();
    return 0;
}

