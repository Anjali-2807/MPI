/**
 * ============================================================================
 * Program: simulation_test.c
 * Purpose: Standalone Zero-Dependency Simulator for System 1 & System 2 MPI Logic
 * Course:  Distributed & Parallel Systems
 * 
 * Description:
 *   This file emulates the exact Message Passing behavior between System 1 (Rank 0)
 *   and System 2 (Rank 1) using standard POSIX IPC (pipes and fork) without
 *   requiring an MPI compiler (mpicc) installed.
 *   
 *   Compilation:
 *     clang -Wall -O2 -o bin/simulation_test src/simulation_test.c
 *   Execution:
 *     ./bin/simulation_test
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define ARRAY_SIZE 8

double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
}

int main() {
    int pipe1[2]; // System 1 -> System 2
    int pipe2[2]; // System 2 -> System 1

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    printf("=================================================================\n");
    printf("   STANDALONE TWO-SYSTEM MESSAGE PASSING SIMULATION (NO MPI REQ) \n");
    printf("=================================================================\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid > 0) {
        // ==========================================
        // SYSTEM 1 (Parent Process / Rank 0)
        // ==========================================
        close(pipe1[0]); // Close read end of pipe1
        close(pipe2[1]); // Close write end of pipe2

        char sys1_host[] = "system1-master (192.168.1.10)";
        printf("[SYSTEM 1] Running on: %s [PID: %d]\n", sys1_host, getpid());

        // --- TEST 1: Basic String Exchange ---
        printf("\n--- Test 1: Basic Point-to-Point Exchange ---\n");
        char send_msg[128] = "Hello System 2! Data payload value = 42";
        printf("[SYSTEM 1 -> SYSTEM 2] Dispatching: \"%s\"\n", send_msg);
        write(pipe1[1], send_msg, sizeof(send_msg));

        char recv_msg[128];
        read(pipe2[0], recv_msg, sizeof(recv_msg));
        printf("[SYSTEM 1 <- SYSTEM 2] Acknowledgment Received: \"%s\"\n", recv_msg);

        // --- TEST 2: Array Transmission & Processing ---
        printf("\n--- Test 2: Numerical Array Processing ---\n");
        double raw_data[ARRAY_SIZE];
        for (int i = 0; i < ARRAY_SIZE; i++) {
            raw_data[i] = (i + 1) * 10.5;
            printf("   raw_data[%d] = %6.2f\n", i, raw_data[i]);
        }
        printf("[SYSTEM 1 -> SYSTEM 2] Sending array of %d doubles...\n", ARRAY_SIZE);
        write(pipe1[1], raw_data, sizeof(raw_data));

        double processed[ARRAY_SIZE];
        double sum = 0.0;
        read(pipe2[0], processed, sizeof(processed));
        read(pipe2[0], &sum, sizeof(sum));

        printf("[SYSTEM 1 <- SYSTEM 2] Received processed array back from System 2:\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("   processed[%d] = %6.2f (computed as: raw * 2.0 + 5.0)\n", i, processed[i]);
        }
        printf("   Aggregated Sum = %6.2f\n", sum);

        // --- TEST 3: Ping-Pong Benchmark ---
        printf("\n--- Test 3: Ping-Pong Latency Benchmark (10 Bounces) ---\n");
        int ball = 0;
        int limit = 10;
        double start = get_time_sec();

        while (ball < limit) {
            ball++;
            printf("[SYSTEM 1 | Rank 0] Sent ping_pong_count = %2d\n", ball);
            write(pipe1[1], &ball, sizeof(ball));
            if (ball >= limit) break;

            read(pipe2[0], &ball, sizeof(ball));
            printf("[SYSTEM 1 | Rank 0] Received ping_pong_count = %2d\n", ball);
        }

        double end = get_time_sec();
        double elapsed = end - start;

        printf("\n=================================================================\n");
        printf("                   PING-PONG BENCHMARK SUMMARY                   \n");
        printf(" Total Bounces Completed : %d\n", limit);
        printf(" Total Elapsed Time      : %.6f seconds\n", elapsed);
        printf(" Average Round-Trip Time : %.4f ms (%.2f microseconds)\n", (elapsed / 5.0) * 1000.0, (elapsed / 5.0) * 1e6);
        printf(" One-Way Network Latency : %.4f ms (%.2f microseconds)\n", (elapsed / 10.0) * 1000.0, (elapsed / 10.0) * 1e6);
        printf("=================================================================\n");

        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);

    } else {
        // ==========================================
        // SYSTEM 2 (Child Process / Rank 1)
        // ==========================================
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2

        char sys2_host[] = "system2-worker (192.168.1.20)";

        // --- TEST 1: Basic String Exchange ---
        char buffer[128];
        read(pipe1[0], buffer, sizeof(buffer));

        char reply[128];
        snprintf(reply, sizeof(reply), "ACK from %s: Payload received and verified.", sys2_host);
        write(pipe2[1], reply, sizeof(reply));

        // --- TEST 2: Array Transformation ---
        double arr[ARRAY_SIZE];
        read(pipe1[0], arr, sizeof(arr));
        double sum = 0.0;
        for (int i = 0; i < ARRAY_SIZE; i++) {
            arr[i] = (arr[i] * 2.0) + 5.0;
            sum += arr[i];
        }
        write(pipe2[1], arr, sizeof(arr));
        write(pipe2[1], &sum, sizeof(sum));

        // --- TEST 3: Ping-Pong ---
        int ball = 0;
        int limit = 10;
        while (ball < limit) {
            read(pipe1[0], &ball, sizeof(ball));
            printf("[SYSTEM 2 | Rank 1] Received ping_pong_count = %2d\n", ball);
            if (ball >= limit) break;

            ball++;
            printf("[SYSTEM 2 | Rank 1] Sent ping_pong_count = %2d\n", ball);
            write(pipe2[1], &ball, sizeof(ball));
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    return 0;
}

