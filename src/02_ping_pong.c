/*
 * Program 2: Ping-Pong Communication and Latency Measurement
 * Author: Anjali Tiwari
 * Course: Parallel and Distributed Computing
 * Instructor: Dr. Sonal Yadav
 *
 * Description:
 *   Bounces a counter back and forth between System 1 and System 2
 *   to measure network round-trip time (RTT) and one-way latency.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define PING_PONG_LIMIT 10

int main(int argc, char *argv[]) {
    int rank, size;
    int len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int count = 0;
    int partner;

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

    // Synchronize both systems before starting benchmark
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    while (count < PING_PONG_LIMIT) {
        if (rank == count % 2) {
            count++;
            MPI_Send(&count, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            printf("[Host: %-15s | Rank %d] Sent ping_pong_count = %2d to Rank %d\n",
                   hostname, rank, count, partner);
        } else {
            MPI_Recv(&count, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("[Host: %-15s | Rank %d] Received ping_pong_count = %2d from Rank %d\n",
                   hostname, rank, count, partner);
        }
    }

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
