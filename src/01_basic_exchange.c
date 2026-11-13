/**
 * ============================================================================
 * Program: 01_basic_exchange.c
 * Purpose: Basic Point-to-Point Message Exchange between Two Systems using MPI
 * Course:  Distributed & Parallel Systems
 * Author:  Parallel Computing Lab Submission
 * 
 * Description:
 *   Demonstrates standard synchronous blocking communication (MPI_Send and MPI_Recv)
 *   between two distinct networked systems (System 1: Rank 0, System 2: Rank 1).
 *   Each system prints its hostname (processor name) to prove distributed execution.
 *   - System 1 sends a greeting message and payload to System 2.
 *   - System 2 receives, processes, and sends an acknowledgment message back.
 *   - System 1 receives the acknowledgment and verifies the transaction.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MESSAGE_SIZE 128
#define TAG_FORWARD  100
#define TAG_REPLY    200

int main(int argc, char** argv) {
    int rank, size;
    int processor_name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    // Initialize the MPI execution environment
    MPI_Init(&argc, &argv);

    // Get the rank (ID) of the calling process within the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the hostname of the system running this process
    MPI_Get_processor_name(processor_name, &processor_name_len);

    // Validate that exactly two processes are participating
    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Error: This program requires exactly 2 processes (System 1 and System 2).\n");
            fprintf(stderr, "Usage: mpirun -np 2 ./01_basic_exchange\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // ==========================================
        // SYSTEM 1 (Rank 0) Logic
        // ==========================================
        char send_msg[MESSAGE_SIZE];
        char recv_msg[MESSAGE_SIZE];
        int data_val = 42;
        MPI_Status status;

        printf("=================================================================\n");
        printf("[SYSTEM 1] Initialized on Host: '%s' (Rank: %d)\n", processor_name, rank);
        printf("=================================================================\n");

        // Prepare payload message
        snprintf(send_msg, sizeof(send_msg), "Hello System 2! Data payload value = %d", data_val);

        printf("[SYSTEM 1 -> SYSTEM 2] Sending message: \"%s\"\n", send_msg);
        
        // Send message to System 2 (Rank 1)
        MPI_Send(send_msg, (int)strlen(send_msg) + 1, MPI_CHAR, 1, TAG_FORWARD, MPI_COMM_WORLD);

        // Receive response / acknowledgment from System 2 (Rank 1)
        MPI_Recv(recv_msg, MESSAGE_SIZE, MPI_CHAR, 1, TAG_REPLY, MPI_COMM_WORLD, &status);

        printf("[SYSTEM 1 <- SYSTEM 2] Received Reply: \"%s\"\n", recv_msg);
        printf("[SYSTEM 1] Data exchange completed successfully.\n");
        printf("=================================================================\n");

    } else if (rank == 1) {
        // ==========================================
        // SYSTEM 2 (Rank 1) Logic
        // ==========================================
        char recv_msg[MESSAGE_SIZE];
        char reply_msg[MESSAGE_SIZE];
        MPI_Status status;

        printf("-----------------------------------------------------------------\n");
        printf("[SYSTEM 2] Initialized on Host: '%s' (Rank: %d)\n", processor_name, rank);
        printf("-----------------------------------------------------------------\n");

        // Receive message from System 1 (Rank 0)
        MPI_Recv(recv_msg, MESSAGE_SIZE, MPI_CHAR, 0, TAG_FORWARD, MPI_COMM_WORLD, &status);

        printf("[SYSTEM 2 <- SYSTEM 1] Successfully received: \"%s\"\n", recv_msg);

        // Process message and formulate reply
        snprintf(reply_msg, sizeof(reply_msg), "ACK from %s: Payload received and verified.", processor_name);

        printf("[SYSTEM 2 -> SYSTEM 1] Sending acknowledgment: \"%s\"\n", reply_msg);

        // Send reply back to System 1 (Rank 0)
        MPI_Send(reply_msg, (int)strlen(reply_msg) + 1, MPI_CHAR, 0, TAG_REPLY, MPI_COMM_WORLD);

        printf("[SYSTEM 2] Transaction finished successfully.\n");
        printf("-----------------------------------------------------------------\n");
    }

    // Terminate MPI execution environment
    MPI_Finalize();
    return 0;
}

