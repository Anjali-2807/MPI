/*
 * Program 1: Basic Point-to-Point MPI Communication
 * Author: Anjali Tiwari
 * Course: Parallel and Distributed Computing
 * Instructor: Dr. Sonal Yadav
 *
 * Description:
 *   System 1 (Rank 0) sends a message and payload to System 2 (Rank 1).
 *   System 2 receives the data and sends back an acknowledgment.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define BUFFER_SIZE 128

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
            printf("Usage: mpirun -np 2 bin/01_basic_exchange\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // System 1 (Rank 0)
        char send_msg[BUFFER_SIZE] = "Hello System 2! Data payload value = 42";
        char recv_msg[BUFFER_SIZE];
        MPI_Status status;

        printf("=================================================================\n");
        printf("[SYSTEM 1] Initialized on Host: '%s' (Rank: %d)\n", hostname, rank);
        printf("=================================================================\n");
        printf("[SYSTEM 1 -> SYSTEM 2] Sending message: \"%s\"\n", send_msg);

        // Send message to System 2 (Rank 1)
        MPI_Send(send_msg, strlen(send_msg) + 1, MPI_CHAR, 1, 100, MPI_COMM_WORLD);

        // Wait for reply
        MPI_Recv(recv_msg, BUFFER_SIZE, MPI_CHAR, 1, 200, MPI_COMM_WORLD, &status);

        printf("[SYSTEM 1 <- SYSTEM 2] Received Reply: \"%s\"\n", recv_msg);
        printf("[SYSTEM 1] Data exchange completed successfully.\n");
        printf("=================================================================\n");

    } else if (rank == 1) {
        // System 2 (Rank 1)
        char recv_msg[BUFFER_SIZE];
        char reply_msg[BUFFER_SIZE];
        MPI_Status status;

        printf("-----------------------------------------------------------------\n");
        printf("[SYSTEM 2] Initialized on Host: '%s' (Rank: %d)\n", hostname, rank);
        printf("-----------------------------------------------------------------\n");

        // Receive message from System 1 (Rank 0)
        MPI_Recv(recv_msg, BUFFER_SIZE, MPI_CHAR, 0, 100, MPI_COMM_WORLD, &status);
        printf("[SYSTEM 2 <- SYSTEM 1] Successfully received: \"%s\"\n", recv_msg);

        // Prepare acknowledgment
        snprintf(reply_msg, sizeof(reply_msg), "ACK from %s: Payload received and verified.", hostname);
        printf("[SYSTEM 2 -> SYSTEM 1] Sending acknowledgment: \"%s\"\n", reply_msg);

        // Send back to System 1
        MPI_Send(reply_msg, strlen(reply_msg) + 1, MPI_CHAR, 0, 200, MPI_COMM_WORLD);

        printf("[SYSTEM 2] Transaction finished successfully.\n");
        printf("-----------------------------------------------------------------\n");
    }

    MPI_Finalize();
    return 0;
}
