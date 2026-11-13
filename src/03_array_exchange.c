/*
 * Program 3: Numerical Array Exchange and Distributed Computation
 * Author: Anjali Tiwari
 * Course: Parallel and Distributed Computing
 * Instructor: Dr. Sonal Yadav
 *
 * Description:
 *   System 1 generates an array of floats and sends it to System 2.
 *   System 2 computes a linear transformation (y = 2*x + 5) and sum reduction,
 *   then sends the processed array and sum back to System 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 8

int main(int argc, char *argv[]) {
    int rank, size;
    int len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

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

    if (rank == 0) {
        // System 1 (Sender / Master)
        double raw_data[ARRAY_SIZE];
        double processed_data[ARRAY_SIZE];
        double sum = 0.0;
        MPI_Status status;

        printf("=================================================================\n");
        printf("[SYSTEM 1] Running on: %s\n", hostname);
        printf("[SYSTEM 1] Generating raw input array:\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            raw_data[i] = (i + 1) * 10.5;
            printf("   raw_data[%d] = %6.2f\n", i, raw_data[i]);
        }

        printf("\n[SYSTEM 1 -> SYSTEM 2] Dispatching %d elements to System 2...\n", ARRAY_SIZE);
        MPI_Send(raw_data, ARRAY_SIZE, MPI_DOUBLE, 1, 10, MPI_COMM_WORLD);

        // Receive processed results back from System 2
        MPI_Recv(processed_data, ARRAY_SIZE, MPI_DOUBLE, 1, 20, MPI_COMM_WORLD, &status);
        MPI_Recv(&sum, 1, MPI_DOUBLE, 1, 30, MPI_COMM_WORLD, &status);

        printf("\n[SYSTEM 1 <- SYSTEM 2] Received processed results from System 2:\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("   processed[%d] = %6.2f (computed as: raw * 2.0 + 5.0)\n", i, processed_data[i]);
        }
        printf("   Sum of processed elements = %6.2f\n", sum);
        printf("=================================================================\n");

    } else if (rank == 1) {
        // System 2 (Receiver / Worker)
        double buffer[ARRAY_SIZE];
        double sum = 0.0;
        MPI_Status status;

        printf("-----------------------------------------------------------------\n");
        printf("[SYSTEM 2] Worker node running on: %s\n", hostname);

        // Receive array from System 1
        MPI_Recv(buffer, ARRAY_SIZE, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &status);
        printf("[SYSTEM 2] Received raw array from System 1. Performing computations...\n");

        // Compute transformation: y = 2*x + 5
        for (int i = 0; i < ARRAY_SIZE; i++) {
            buffer[i] = (buffer[i] * 2.0) + 5.0;
            sum += buffer[i];
        }

        printf("[SYSTEM 2] Computation complete. Transmitting results back to System 1...\n");
        MPI_Send(buffer, ARRAY_SIZE, MPI_DOUBLE, 0, 20, MPI_COMM_WORLD);
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD);

        printf("[SYSTEM 2] Completed successfully.\n");
        printf("-----------------------------------------------------------------\n");
    }

    MPI_Finalize();
    return 0;
}
