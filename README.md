# MPI Parallel Programs - Data Exchange Between Two Systems

**Author:** Anjali Tiwari  
**Course:** Parallel and Distributed Computing  
**Instructor:** Dr. Sonal Yadav  

---

## Overview

This project implements parallel programs using the Message Passing Interface (MPI) to exchange data across two separate computer systems connected over a local network.

- **System 1 (Master / Rank 0):** `192.168.1.10` (`system1-master`)
- **System 2 (Worker / Rank 1):** `192.168.1.20` (`system2-worker`)

The programs demonstrate fundamental distributed communication patterns:
1. **Basic Point-to-Point Communication:** Sending a message payload and receiving an acknowledgment reply.
2. **Ping-Pong Benchmark:** Bouncing a message back and forth to measure network round-trip time (RTT) and one-way latency.
3. **Array Exchange & Computation:** Sending an array of numbers, computing transformations and sum on the worker node, and returning the results.
4. **Non-Blocking Communication:** Overlapping computation and communication using `MPI_Isend` and `MPI_Irecv` to prevent deadlocks.

---

## Project Structure

```
├── Makefile                    # Compiles all programs
├── hostfile                    # Defines IPs and slots for System 1 and System 2
├── README.md                   # Setup guide and documentation
├── src/
│   ├── 01_basic_exchange.c     # Point-to-point send/recv
│   ├── 02_ping_pong.c          # Latency & RTT benchmark
│   ├── 03_array_exchange.c     # Numerical array processing
│   ├── 04_nonblocking_exchange.c # Asynchronous communication
│   └── mpi_exchange.py         # Python mpi4py reference
├── scripts/
│   ├── setup_ssh.sh            # Helper to configure passwordless SSH
│   ├── run_cluster.sh          # Helper to run across 2 machines
│   └── run_local.sh            # Helper to test locally on 1 machine
└── screenshots/                # Execution and terminal output screenshots
    ├── screenshot1_network_ssh.png
    ├── screenshot2_compilation.png
    ├── screenshot3_basic_exchange.png
    ├── screenshot4_ping_pong.png
    ├── screenshot5_array_exchange.png
    └── screenshot6_nonblocking.png
```

---

## Setup Instructions

### 1. Prerequisites
Both systems should have OpenMPI, GCC/build tools, and OpenSSH installed:

**On Ubuntu/Debian (both systems):**
```bash
sudo apt update
sudo apt install build-essential openmpi-bin libopenmpi-dev openssh-server -y
```

**On macOS:**
```bash
brew install open-mpi
```

### 2. Network Configuration
Add the IP addresses and hostnames to `/etc/hosts` on both systems so they can resolve each other:
```
192.168.1.10   system1-master
192.168.1.20   system2-worker
```

Test network connectivity from System 1:
```bash
ping -c 2 system2-worker
```

### 3. Passwordless SSH Setup
MPI launches processes on remote nodes using SSH without entering a password every time.

From System 1, generate an SSH key and copy it to System 2:
```bash
ssh-keygen -t rsa -b 4096 -N "" -f ~/.ssh/id_rsa
ssh-copy-id mpiuser@system2-worker
ssh-copy-id mpiuser@system1-master
```

Verify that SSH logs in without asking for a password:
```bash
ssh mpiuser@system2-worker "hostname"
```

### 4. Matching Directory Paths
OpenMPI expects the project directory and compiled binaries to be in the exact same path on both systems (e.g. `/home/mpiuser/MPI`).

You can copy the folder to System 2 using `scp`:
```bash
scp -r ~/MPI mpiuser@system2-worker:~/
```

---

## Compiling the Programs

Run `make` in the project root:
```bash
make
```

This compiles all four programs into the `bin/` directory:
- `bin/01_basic_exchange`
- `bin/02_ping_pong`
- `bin/03_array_exchange`
- `bin/04_nonblocking_exchange`

If you are not using a shared NFS folder, sync the compiled binaries to System 2:
```bash
scp -r bin/ mpiuser@system2-worker:~/MPI/
```

---

## Running the Programs

### Running Across the Two Systems (Cluster Mode)
Run the commands from System 1 using the `hostfile`:

```bash
# 1. Basic Send / Receive
mpirun --hostfile hostfile -np 2 bin/01_basic_exchange

# 2. Ping-Pong Latency Benchmark
mpirun --hostfile hostfile -np 2 bin/02_ping_pong

# 3. Array Exchange and Processing
mpirun --hostfile hostfile -np 2 bin/03_array_exchange

# 4. Non-Blocking Communication
mpirun --hostfile hostfile -np 2 bin/04_nonblocking_exchange
```

*Note:* If your machines have multiple network adapters (e.g. WiFi and Ethernet), specify the interface subnet:
```bash
mpirun --hostfile hostfile -mca btl_tcp_if_include 192.168.1.0/24 -np 2 bin/01_basic_exchange
```

### Running Locally (Testing on 1 machine)
To test the code locally using two processes on a single laptop/desktop:
```bash
mpirun -np 2 bin/01_basic_exchange
mpirun -np 2 bin/02_ping_pong
mpirun -np 2 bin/03_array_exchange
mpirun -np 2 bin/04_nonblocking_exchange
```

---

## Execution Results & Output Screenshots

### 1. Network Connectivity & SSH Setup
Verifying network ping between System 1 (`system1-master`) and System 2 (`system2-worker`), followed by passwordless SSH login:

![Network & SSH Setup](screenshots/screenshot1_network_ssh.png)

---

### 2. Hostfile Configuration & Compilation
Checking the hostfile configuration and compiling all binaries with `make`:

![Compilation](screenshots/screenshot2_compilation.png)

---

### 3. Program 1: Basic Point-to-Point Exchange
System 1 (Rank 0) sends a message and payload to System 2 (Rank 1). System 2 receives it, verifies the payload, and sends back an acknowledgment reply:

![Basic Exchange](screenshots/screenshot3_basic_exchange.png)

---

### 4. Program 2: Ping-Pong Benchmark & Latency Measurement
A counter is bounced back and forth 10 times between the two systems. Using `MPI_Wtime()`, we measure total time, average round-trip time (RTT), and one-way network latency:

![Ping Pong Benchmark](screenshots/screenshot4_ping_pong.png)

**Benchmark Results:**
- Total Bounces: 10
- Total Elapsed Time: 0.001224 seconds
- Average Round-Trip Time (RTT): 0.2448 ms (244.80 µs)
- One-Way Network Latency: 0.1224 ms (122.40 µs)

---

### 5. Program 3: Numerical Array Processing
System 1 initializes an array of 8 floating-point values and sends it to System 2. System 2 performs element-wise calculations ($y = 2x + 5$) and calculates the sum, then sends the processed array and sum back to System 1:

![Array Exchange](screenshots/screenshot5_array_exchange.png)

---

### 6. Program 4: Asynchronous Non-Blocking Communication
Both systems initiate non-blocking receives (`MPI_Irecv`) and sends (`MPI_Isend`), then perform local CPU computations while communication is handled in the background. `MPI_Waitall` ensures both operations complete without deadlocks:

![Non-Blocking Exchange](screenshots/screenshot6_nonblocking.png)

---

## Troubleshooting Tips

1. **Permission Denied (publickey):**
   - Ensure you ran `ssh-copy-id` to both `system2-worker` and `system1-master` (localhost).
   - Check that SSH service is active: `sudo systemctl status ssh`.

2. **MPI Executable Not Found:**
   - The compiled binaries must be located at the exact same directory path on System 2 as they are on System 1. Copy the `bin/` folder over or use NFS.

3. **Processes Hang / Firewall Block:**
   - OpenMPI uses random dynamic TCP ports. If it hangs, ensure the firewall allows traffic on the local subnet:
     `sudo ufw allow from 192.168.1.0/24`
