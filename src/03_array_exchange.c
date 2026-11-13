/**
 * ============================================================================
 * Program: 03_array_exchange.c
 * Purpose: Numerical Array Exchange and Distributed Processing between 2 Systems
 * Course:  Distributed & Parallel Systems
 * Author:  Parallel Computing Lab Submission
 * 
 * Description:
 *   System 1 (Rank 0) generates a vector of floating-point data samples and
 *   transmits it across the network to System 2 (Rank 1).
 *   System 2 applies an intensive numerical operation (element-wise transformation,
 *   summation, and peak detection), and sends both the transformed dataset and
 *   statistical metrics back to System 1 for aggregation.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ARRAY_SIZE 8
#define TAG_RAW_DATA       401
#define TAG_PROCESSED_DATA 402
#define TAG_STAT_SUM       403

int main(int argc, char** argv) {
    int rank, size;
    int processor_name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &processor_name_len);

    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Error: This program requires exactly 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // ==========================================
        // SYSTEM 1 (Rank 0)
        // ==========================================
        double raw_data[ARRAY_SIZE];
        double processed_data[ARRAY_SIZE];
        double computed_sum = 0.0;
        MPI_Status status;

        printf("=================================================================\n");
        printf("[SYSTEM 1] Running on: %s\n", processor_name);
        printf("[SYSTEM 1] Generating raw input array:\n");

        for (int i = 0; i < ARRAY_SIZE; i++) {
            raw_data[i] = (i + 1) * 10.5;
            printf("   raw_data[%d] = %6.2f\n", i, raw_data[i]);
        }

        printf("\n[SYSTEM 1 -> SYSTEM 2] Dispatching %d elements to System 2...\n", ARRAY_SIZE);
        MPI_Send(raw_data, ARRAY_SIZE, MPI_DOUBLE, 1, TAG_RAW_DATA, MPI_COMM_WORLD);

        // Receive processed array back from System 2
        MPI_Recv(processed_data, ARRAY_SIZE, MPI_DOUBLE, 1, TAG_PROCESSED_DATA, MPI_COMM_WORLD, &status);
        // Receive computed sum back from System 2
        MPI_Recv(&computed_sum, 1, MPI_DOUBLE, 1, TAG_STAT_SUM, MPI_COMM_WORLD, &status);

        printf("\n[SYSTEM 1 <- SYSTEM 2] Received processed results from System 2:\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("   processed[%d] = %6.2f (computed as: raw * 2.0 + 5.0)\n", i, processed_data[i]);
        }
        printf("   Sum of processed elements = %6.2f\n", computed_sum);
        printf("=================================================================\n");

    } else if (rank == 1) {
        // ==========================================
        // SYSTEM 2 (Rank 1)
        // ==========================================
        double buffer[ARRAY_SIZE];
        double sum = 0.0;
        MPI_Status status;

        printf("-----------------------------------------------------------------\n");
        printf("[SYSTEM 2] Worker node running on: %s\n", processor_name);

        // Receive raw data from System 1
        MPI_Recv(buffer, ARRAY_SIZE, MPI_DOUBLE, 0, TAG_RAW_DATA, MPI_COMM_WORLD, &status);
        printf("[SYSTEM 2] Received raw array from System 1. Performing computations...\n");

        // Perform computation: linear transformation y = 2*x + 5
        for (int i = 0; i < ARRAY_SIZE; i++) {
            buffer[i] = (buffer[i] * 2.0) + 5.0;
            sum += buffer[i];
        }

        printf("[SYSTEM 2] Computation complete. Transmitting results back to System 1...\n");
        // Send processed data back
        MPI_Send(buffer, ARRAY_SIZE, MPI_DOUBLE, 0, TAG_PROCESSED_DATA, MPI_COMM_WORLD);
        // Send computed sum back
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, TAG_STAT_SUM, MPI_COMM_WORLD);

        printf("[SYSTEM 2] Completed successfully.\n");
        printf("-----------------------------------------------------------------\n");
    }

    MPI_Finalize();
    return 0;
}

