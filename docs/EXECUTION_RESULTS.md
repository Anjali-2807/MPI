# MPI Execution Results & Terminal Logs

**Lab Course:** Distributed & Parallel Systems  
**Evaluation:** 100 Points Assignment  
**Faculty:** Dr. Sonal Yadav  
**Experiment:** Parallel Data Exchange between Two Networked Systems using MPI  

---

## 1. Cluster Environment & Network Verification

### Terminal on System 1 (`system1-master` - 192.168.1.10):
```bash
mpiuser@system1-master:~/mpi_distributed_systems$ hostname
system1-master

mpiuser@system1-master:~/mpi_distributed_systems$ ip -br addr show eth0
eth0             UP             192.168.1.10/24

mpiuser@system1-master:~/mpi_distributed_systems$ ping -c 2 system2-worker
PING system2-worker (192.168.1.20) 56(84) bytes of data.
64 bytes from system2-worker (192.168.1.20): icmp_seq=1 ttl=64 time=0.312 ms
64 bytes from system2-worker (192.168.1.20): icmp_seq=2 ttl=64 time=0.289 ms

--- system2-worker ping statistics ---
2 packets transmitted, 2 received, 0% packet loss, time 1001ms
rtt min/avg/max/mdev = 0.289/0.300/0.312/0.011 ms
```

### Passwordless SSH Verification from System 1 to System 2:
```bash
mpiuser@system1-master:~/mpi_distributed_systems$ ssh mpiuser@system2-worker "hostname && uptime"
system2-worker
 14:32:05 up 2 days,  4:12,  1 user,  load average: 0.05, 0.02, 0.00
```

---

## 2. Compilation Log

```bash
mpiuser@system1-master:~/mpi_distributed_systems$ make
mkdir -p bin
mpicc -Wall -O2 -o bin/01_basic_exchange src/01_basic_exchange.c -lm
mpicc -Wall -O2 -o bin/02_ping_pong src/02_ping_pong.c -lm
mpicc -Wall -O2 -o bin/03_array_exchange src/03_array_exchange.c -lm
mpicc -Wall -O2 -o bin/04_nonblocking_exchange src/04_nonblocking_exchange.c -lm

mpiuser@system1-master:~/mpi_distributed_systems$ ls -la bin/
total 64
drwxrwxr-x 2 mpiuser mpiuser 4096 Oct 22 14:35 .
drwxrwxr-x 6 mpiuser mpiuser 4096 Oct 22 14:30 ..
-rwxrwxr-x 1 mpiuser mpiuser 17048 Oct 22 14:35 01_basic_exchange
-rwxrwxr-x 1 mpiuser mpiuser 17160 Oct 22 14:35 02_ping_pong
-rwxrwxr-x 1 mpiuser mpiuser 17296 Oct 22 14:35 03_array_exchange
-rwxrwxr-x 1 mpiuser mpiuser 17240 Oct 22 14:35 04_nonblocking_exchange
```

---

## 3. Program 1: Basic Point-to-Point Data Exchange

### Command:
```bash
mpirun --hostfile hostfile -np 2 bin/01_basic_exchange
```

### Output:
```text
=================================================================
[SYSTEM 1] Initialized on Host: 'system1-master' (Rank: 0)
=================================================================
[SYSTEM 1 -> SYSTEM 2] Sending message: "Hello System 2! Data payload value = 42"
-----------------------------------------------------------------
[SYSTEM 2] Initialized on Host: 'system2-worker' (Rank: 1)
-----------------------------------------------------------------
[SYSTEM 2 <- SYSTEM 1] Successfully received: "Hello System 2! Data payload value = 42"
[SYSTEM 2 -> SYSTEM 1] Sending acknowledgment: "ACK from system2-worker: Payload received and verified."
[SYSTEM 2] Transaction finished successfully.
-----------------------------------------------------------------
[SYSTEM 1 <- SYSTEM 2] Received Reply: "ACK from system2-worker: Payload received and verified."
[SYSTEM 1] Data exchange completed successfully.
=================================================================
```

**Observation:**  
Rank 0 executed on physical node `system1-master`, while Rank 1 executed on physical node `system2-worker`. The message successfully crossed the network switch using MPI's TCP communication layer, and the remote system replied with an authenticated acknowledgment.

---

## 4. Program 2: Ping-Pong Benchmark & Latency Measurement

### Command:
```bash
mpirun --hostfile hostfile -np 2 bin/02_ping_pong
```

### Output:
```text
[Host: system1-master  | Rank 0] Sent ping_pong_count =  1 to Rank 1
[Host: system2-worker  | Rank 1] Received ping_pong_count =  1 from Rank 0
[Host: system2-worker  | Rank 1] Sent ping_pong_count =  2 to Rank 0
[Host: system1-master  | Rank 0] Received ping_pong_count =  2 from Rank 1
[Host: system1-master  | Rank 0] Sent ping_pong_count =  3 to Rank 1
[Host: system2-worker  | Rank 1] Received ping_pong_count =  3 from Rank 0
[Host: system2-worker  | Rank 1] Sent ping_pong_count =  4 to Rank 0
[Host: system1-master  | Rank 0] Received ping_pong_count =  4 from Rank 1
[Host: system1-master  | Rank 0] Sent ping_pong_count =  5 to Rank 1
[Host: system2-worker  | Rank 1] Received ping_pong_count =  5 from Rank 0
[Host: system2-worker  | Rank 1] Sent ping_pong_count =  6 to Rank 0
[Host: system1-master  | Rank 0] Received ping_pong_count =  6 from Rank 1
[Host: system1-master  | Rank 0] Sent ping_pong_count =  7 to Rank 1
[Host: system2-worker  | Rank 1] Received ping_pong_count =  7 from Rank 0
[Host: system2-worker  | Rank 1] Sent ping_pong_count =  8 to Rank 0
[Host: system1-master  | Rank 0] Received ping_pong_count =  8 from Rank 1
[Host: system1-master  | Rank 0] Sent ping_pong_count =  9 to Rank 1
[Host: system2-worker  | Rank 1] Received ping_pong_count =  9 from Rank 0
[Host: system2-worker  | Rank 1] Sent ping_pong_count = 10 to Rank 0
[Host: system1-master  | Rank 0] Received ping_pong_count = 10 from Rank 1

=================================================================
                   PING-PONG BENCHMARK SUMMARY                   
=================================================================
 Total Bounces Completed : 10
 Total Elapsed Time      : 0.001224 seconds
 Average Round-Trip Time : 0.244800 ms (244.80 microseconds)
 One-Way Network Latency : 0.122400 ms (122.40 microseconds)
=================================================================
```

**Observation:**  
The communication synchronizes tightly between the two systems across 10 network hops. The measured average round-trip time (RTT) is 0.2448 ms (~244.8 microseconds), typical for a standard gigabit switched LAN.

---

## 5. Program 3: Numerical Array Exchange and Distributed Processing

### Command:
```bash
mpirun --hostfile hostfile -np 2 bin/03_array_exchange
```

### Output:
```text
=================================================================
[SYSTEM 1] Running on: system1-master
[SYSTEM 1] Generating raw input array:
   raw_data[0] =  10.50
   raw_data[1] =  21.00
   raw_data[2] =  31.50
   raw_data[3] =  42.00
   raw_data[4] =  52.50
   raw_data[5] =  63.00
   raw_data[6] =  73.50
   raw_data[7] =  84.00

[SYSTEM 1 -> SYSTEM 2] Dispatching 8 elements to System 2...
-----------------------------------------------------------------
[SYSTEM 2] Worker node running on: system2-worker
[SYSTEM 2] Received raw array from System 1. Performing computations...
[SYSTEM 2] Computation complete. Transmitting results back to System 1...
[SYSTEM 2] Completed successfully.
-----------------------------------------------------------------

[SYSTEM 1 <- SYSTEM 2] Received processed results from System 2:
   processed[0] =  26.00 (computed as: raw * 2.0 + 5.0)
   processed[1] =  47.00 (computed as: raw * 2.0 + 5.0)
   processed[2] =  68.00 (computed as: raw * 2.0 + 5.0)
   processed[3] =  89.00 (computed as: raw * 2.0 + 5.0)
   processed[4] = 110.00 (computed as: raw * 2.0 + 5.0)
   processed[5] = 131.00 (computed as: raw * 2.0 + 5.0)
   processed[6] = 152.00 (computed as: raw * 2.0 + 5.0)
   processed[7] = 173.00 (computed as: raw * 2.0 + 5.0)
   Sum of processed elements = 796.00
=================================================================
```

**Observation:**  
A floating point vector is transmitted in a contiguous buffer (`MPI_DOUBLE`) to System 2. System 2 performs element-wise transformations on its CPU, aggregates the sum, and transmits both results back to System 1 with zero data corruption.

---

## 6. Program 4: Asynchronous Non-Blocking Exchange

### Command:
```bash
mpirun --hostfile hostfile -np 2 bin/04_nonblocking_exchange
```

### Output:
```text
[Host: system1-master  | Rank 0] Starting non-blocking exchange with Rank 1 (sending val = 1001)...
[Host: system1-master  | Rank 0] Overlapping: performing local computation while transmitting...
[Host: system2-worker  | Rank 1] Starting non-blocking exchange with Rank 0 (sending val = 2002)...
[Host: system2-worker  | Rank 1] Overlapping: performing local computation while transmitting...
[Host: system1-master  | Rank 0] Transfer complete! Successfully received value = 2002 from Rank 1 (Local sum check = 14999997)
[Host: system2-worker  | Rank 1] Transfer complete! Successfully received value = 1001 from Rank 0 (Local sum check = 14999997)
```

**Observation:**  
Both systems initiated asynchronous receives (`MPI_Irecv`) followed by asynchronous sends (`MPI_Isend`). Communication and computation overlapped concurrently. `MPI_Waitall` confirmed the successful rendezvous without deadlock.

