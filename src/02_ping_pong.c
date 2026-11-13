/**
 * ============================================================================
 * Program: 02_ping_pong.c
 * Purpose: Ping-Pong Benchmark & Latency Measurement between Two Systems
 * Course:  Distributed & Parallel Systems
 * Author:  Parallel Computing Lab Submission
 * 
 * Description:
 *   Implements a classic Ping-Pong communication pattern between System 1 (Rank 0)
 *   and System 2 (Rank 1). A counter ("ball") is bounced back and forth between
 *   the two network nodes until a limit is reached.
 *   
 *   Measures:
 *   - Total elapsed execution time using MPI_Wtime()
 *   - Average round-trip time (RTT)
 *   - One-way network latency between System 1 and System 2
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define PING_PONG_LIMIT 10
#define TAG_PING_PONG   300

int main(int argc, char** argv) {
    int rank, size;
    int processor_name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int ping_pong_count = 0;
    int partner_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &processor_name_len);

    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Error: Ping-pong requires exactly 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Determine partner: Rank 0's partner is 1, Rank 1's partner is 0
    partner_rank = (rank + 1) % 2;

    // Synchronize both systems before starting the timer
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    while (ping_pong_count < PING_PONG_LIMIT) {
        if (rank == ping_pong_count % 2) {
            // It is this rank's turn to hit the ball (increment and send)
            ping_pong_count++;
            MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, TAG_PING_PONG, MPI_COMM_WORLD);
            printf("[Host: %-15s | Rank %d] Sent ping_pong_count = %2d to Rank %d\n",
                   processor_name, rank, ping_pong_count, partner_rank);
        } else {
            // Wait to receive the ball from the partner
            MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, TAG_PING_PONG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("[Host: %-15s | Rank %d] Received ping_pong_count = %2d from Rank %d\n",
                   processor_name, rank, ping_pong_count, partner_rank);
        }
    }

    // Synchronize both systems after completing all exchanges
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    double total_time = end_time - start_time;

    if (rank == 0) {
        double avg_rtt = total_time / (PING_PONG_LIMIT / 2.0);
        double one_way_latency = total_time / (double)PING_PONG_LIMIT;

        printf("\n=================================================================\n");
        printf("                   PING-PONG BENCHMARK SUMMARY                   \n");
        printf("=================================================================\n");
        printf(" Total Bounces Completed : %d\n", PING_PONG_LIMIT);
        printf(" Total Elapsed Time      : %.6f seconds\n", total_time);
        printf(" Average Round-Trip Time : %.6f ms (%.2f microseconds)\n", avg_rtt * 1000.0, avg_rtt * 1e6);
        printf(" One-Way Network Latency : %.6f ms (%.2f microseconds)\n", one_way_latency * 1000.0, one_way_latency * 1e6);
        printf("=================================================================\n");
    }

    MPI_Finalize();
    return 0;
}

