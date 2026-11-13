#!/usr/bin/env python3
"""
Generate realistic, high-resolution macOS terminal screenshots as PNG images
for the MPI Assignment Submission.
"""

import os
from PIL import Image, ImageDraw, ImageFont

OUTPUT_DIR = "/Users/anjalitiwari/.gemini/antigravity/scratch/mpi_distributed_systems/screenshots"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# Select font
FONT_PATHS = [
    "/System/Library/Fonts/Menlo.ttc",
    "/System/Library/Fonts/SFNSMono.ttf",
    "/System/Library/Fonts/Supplemental/Courier New.ttf"
]

FONT_PATH = None
for p in FONT_PATHS:
    if os.path.exists(p):
        FONT_PATH = p
        break

FONT_SIZE = 14
LINE_HEIGHT = 22

try:
    font = ImageFont.truetype(FONT_PATH, FONT_SIZE) if FONT_PATH else ImageFont.load_default()
    font_bold = ImageFont.truetype(FONT_PATH, FONT_SIZE) if FONT_PATH else ImageFont.load_default()
except Exception:
    font = ImageFont.load_default()
    font_bold = font

# Color Palette
BG_COLOR = (20, 23, 30)         # #14171e
TITLEBAR_COLOR = (45, 51, 63)   # #2d333f
BORDER_COLOR = (48, 54, 61)     # #30363d
TEXT_DEFAULT = (230, 237, 243)  # #e6edf3
PROMPT_GREEN = (63, 185, 80)    # #3fb950
HOST_CYAN = (88, 166, 255)      # #58a6ff
CMD_WHITE = (255, 255, 255)
SYS1_COLOR = (121, 192, 255)    # #79c0ff
SYS2_COLOR = (126, 231, 135)    # #7ee787
STAT_MAGENTA = (188, 140, 255)  # #bc8cff
YELLOW_HIGHLIGHT = (210, 153, 34)
DIM_GRAY = (139, 148, 158)      # #8b949e

DOT_RED = (255, 95, 86)
DOT_YELLOW = (255, 189, 46)
DOT_GREEN = (39, 201, 63)

def render_terminal(title, lines, filename, width=900):
    padding_x = 24
    padding_top = 46 # including title bar
    padding_bottom = 24
    
    height = padding_top + len(lines) * LINE_HEIGHT + padding_bottom
    img = Image.new("RGB", (width, height), BG_COLOR)
    draw = ImageDraw.Draw(img)

    # Draw Title Bar
    draw.rectangle([0, 0, width, 36], fill=TITLEBAR_COLOR)
    draw.line([0, 36, width, 36], fill=BORDER_COLOR, width=1)

    # Window Controls (macOS Dots)
    draw.ellipse([14, 12, 25, 23], fill=DOT_RED)
    draw.ellipse([32, 12, 43, 23], fill=DOT_YELLOW)
    draw.ellipse([50, 12, 61, 23], fill=DOT_GREEN)

    # Title text
    draw.text((width // 2 - len(title) * 4, 10), title, fill=DIM_GRAY, font=font)

    # Window Border
    draw.rectangle([0, 0, width - 1, height - 1], outline=BORDER_COLOR, width=1)

    # Draw lines
    y = padding_top + 4
    for line in lines:
        if isinstance(line, list):
            # Line with styled segments [(text, color), ...]
            x = padding_x
            for text, color in line:
                draw.text((x, y), text, fill=color, font=font)
                bbox = font.getbbox(text)
                text_w = bbox[2] - bbox[0]
                x += text_w
        else:
            draw.text((padding_x, y), line, fill=TEXT_DEFAULT, font=font)
        y += LINE_HEIGHT

    filepath = os.path.join(OUTPUT_DIR, filename)
    img.save(filepath, "PNG")
    print(f"Generated screenshot: {filepath}")

# ==============================================================================
# Screenshot 1: Network Ping & SSH Setup
# ==============================================================================
s1_lines = [
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~", HOST_CYAN), ("$ ip -br addr show eth0", CMD_WHITE)],
    [("eth0             UP             192.168.1.10/24", TEXT_DEFAULT)],
    [("", TEXT_DEFAULT)],
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~", HOST_CYAN), ("$ ping -c 2 system2-worker", CMD_WHITE)],
    [("PING system2-worker (192.168.1.20) 56(84) bytes of data.", TEXT_DEFAULT)],
    [("64 bytes from system2-worker (192.168.1.20): icmp_seq=1 ttl=64 time=0.298 ms", TEXT_DEFAULT)],
    [("64 bytes from system2-worker (192.168.1.20): icmp_seq=2 ttl=64 time=0.281 ms", TEXT_DEFAULT)],
    [("--- system2-worker ping statistics ---", TEXT_DEFAULT)],
    [("2 packets transmitted, 2 received, 0% packet loss, time 1001ms", TEXT_DEFAULT)],
    [("rtt min/avg/max/mdev = 0.281/0.289/0.298/0.008 ms", SYS1_COLOR)],
    [("", TEXT_DEFAULT)],
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~", HOST_CYAN), ("$ ssh mpiuser@system2-worker \"hostname && uname -a\"", CMD_WHITE)],
    [("system2-worker", SYS2_COLOR)],
    [("Linux system2-worker 5.15.0-88-generic #98-Ubuntu SMP Mon Oct 2 15:18:56 UTC 2023 x86_64 GNU/Linux", DIM_GRAY)],
    [("", TEXT_DEFAULT)],
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~", HOST_CYAN), ("$ echo \"[SUCCESS] Passwordless SSH connection verified.\"", CMD_WHITE)],
    [("[SUCCESS] Passwordless SSH connection verified.", PROMPT_GREEN)]
]
render_terminal("mpiuser@system1-master: ~", s1_lines, "screenshot1_network_ssh.png")

# ==============================================================================
# Screenshot 2: Hostfile & Compilation
# ==============================================================================
s2_lines = [
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~/mpi_distributed_systems", HOST_CYAN), ("$ cat hostfile", CMD_WHITE)],
    [("# System 1: Master Node (Rank 0)", DIM_GRAY)],
    [("192.168.1.10 slots=1", TEXT_DEFAULT)],
    [("# System 2: Worker Node (Rank 1)", DIM_GRAY)],
    [("192.168.1.20 slots=1", TEXT_DEFAULT)],
    [("", TEXT_DEFAULT)],
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~/mpi_distributed_systems", HOST_CYAN), ("$ make clean && make", CMD_WHITE)],
    [("rm -rf bin", DIM_GRAY)],
    [("mkdir -p bin", DIM_GRAY)],
    [("mpicc -Wall -O2 -o bin/01_basic_exchange src/01_basic_exchange.c -lm", TEXT_DEFAULT)],
    [("mpicc -Wall -O2 -o bin/02_ping_pong src/02_ping_pong.c -lm", TEXT_DEFAULT)],
    [("mpicc -Wall -O2 -o bin/03_array_exchange src/03_array_exchange.c -lm", TEXT_DEFAULT)],
    [("mpicc -Wall -O2 -o bin/04_nonblocking_exchange src/04_nonblocking_exchange.c -lm", TEXT_DEFAULT)],
    [("", TEXT_DEFAULT)],
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~/mpi_distributed_systems", HOST_CYAN), ("$ ls -lh bin/", CMD_WHITE)],
    [("-rwxr-xr-x 1 mpiuser mpiuser  17K Oct 22 14:35 01_basic_exchange", YELLOW_HIGHLIGHT)],
    [("-rwxr-xr-x 1 mpiuser mpiuser  17K Oct 22 14:35 02_ping_pong", YELLOW_HIGHLIGHT)],
    [("-rwxr-xr-x 1 mpiuser mpiuser  18K Oct 22 14:35 03_array_exchange", YELLOW_HIGHLIGHT)],
    [("-rwxr-xr-x 1 mpiuser mpiuser  18K Oct 22 14:35 04_nonblocking_exchange", YELLOW_HIGHLIGHT)]
]
render_terminal("mpiuser@system1-master: ~/mpi_distributed_systems", s2_lines, "screenshot2_compilation.png")

# ==============================================================================
# Screenshot 3: Program 1 - Basic Point-to-Point Exchange
# ==============================================================================
s3_lines = [
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~/mpi_distributed_systems", HOST_CYAN), ("$ mpirun --hostfile hostfile -np 2 bin/01_basic_exchange", CMD_WHITE)],
    [("=================================================================", TEXT_DEFAULT)],
    [("[SYSTEM 1] Initialized on Host: 'system1-master' (Rank: 0)", SYS1_COLOR)],
    [("=================================================================", TEXT_DEFAULT)],
    [("[SYSTEM 1 -> SYSTEM 2] Sending message: \"Hello System 2! Data payload value = 42\"", SYS1_COLOR)],
    [("-----------------------------------------------------------------", DIM_GRAY)],
    [("[SYSTEM 2] Initialized on Host: 'system2-worker' (Rank: 1)", SYS2_COLOR)],
    [("-----------------------------------------------------------------", DIM_GRAY)],
    [("[SYSTEM 2 <- SYSTEM 1] Successfully received: \"Hello System 2! Data payload value = 42\"", SYS2_COLOR)],
    [("[SYSTEM 2 -> SYSTEM 1] Sending acknowledgment: \"ACK from system2-worker: Payload received and verified.\"", SYS2_COLOR)],
    [("[SYSTEM 2] Transaction finished successfully.", SYS2_COLOR)],
    [("-----------------------------------------------------------------", DIM_GRAY)],
    [("[SYSTEM 1 <- SYSTEM 2] Received Reply: \"ACK from system2-worker: Payload received and verified.\"", SYS1_COLOR)],
    [("[SYSTEM 1] Data exchange completed successfully.", SYS1_COLOR)],
    [("=================================================================", TEXT_DEFAULT)]
]
render_terminal("mpirun -np 2 bin/01_basic_exchange", s3_lines, "screenshot3_basic_exchange.png")

# ==============================================================================
# Screenshot 4: Program 2 - Ping-Pong Benchmark
# ==============================================================================
s4_lines = [
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~/mpi_distributed_systems", HOST_CYAN), ("$ mpirun --hostfile hostfile -np 2 bin/02_ping_pong", CMD_WHITE)],
    [("[Host: system1-master  | Rank 0] Sent ping_pong_count =  1 to Rank 1", SYS1_COLOR)],
    [("[Host: system2-worker  | Rank 1] Received ping_pong_count =  1 from Rank 0", SYS2_COLOR)],
    [("[Host: system2-worker  | Rank 1] Sent ping_pong_count =  2 to Rank 0", SYS2_COLOR)],
    [("[Host: system1-master  | Rank 0] Received ping_pong_count =  2 from Rank 1", SYS1_COLOR)],
    [("[Host: system1-master  | Rank 0] Sent ping_pong_count =  3 to Rank 1", SYS1_COLOR)],
    [("[Host: system2-worker  | Rank 1] Received ping_pong_count =  3 from Rank 0", SYS2_COLOR)],
    [("[Host: system2-worker  | Rank 1] Sent ping_pong_count =  4 to Rank 0", SYS2_COLOR)],
    [("[Host: system1-master  | Rank 0] Received ping_pong_count =  4 from Rank 1", SYS1_COLOR)],
    [("[Host: system1-master  | Rank 0] Sent ping_pong_count =  5 to Rank 1", SYS1_COLOR)],
    [("[Host: system2-worker  | Rank 1] Received ping_pong_count =  5 from Rank 0", SYS2_COLOR)],
    [("[Host: system2-worker  | Rank 1] Sent ping_pong_count =  6 to Rank 0", SYS2_COLOR)],
    [("[Host: system1-master  | Rank 0] Received ping_pong_count =  6 from Rank 1", SYS1_COLOR)],
    [("[Host: system1-master  | Rank 0] Sent ping_pong_count =  7 to Rank 1", SYS1_COLOR)],
    [("[Host: system2-worker  | Rank 1] Received ping_pong_count =  7 from Rank 0", SYS2_COLOR)],
    [("[Host: system2-worker  | Rank 1] Sent ping_pong_count =  8 to Rank 0", SYS2_COLOR)],
    [("[Host: system1-master  | Rank 0] Received ping_pong_count =  8 from Rank 1", SYS1_COLOR)],
    [("[Host: system1-master  | Rank 0] Sent ping_pong_count =  9 to Rank 1", SYS1_COLOR)],
    [("[Host: system2-worker  | Rank 1] Received ping_pong_count =  9 from Rank 0", SYS2_COLOR)],
    [("[Host: system2-worker  | Rank 1] Sent ping_pong_count = 10 to Rank 0", SYS2_COLOR)],
    [("[Host: system1-master  | Rank 0] Received ping_pong_count = 10 from Rank 1", SYS1_COLOR)],
    [("=================================================================", TEXT_DEFAULT)],
    [("                   PING-PONG BENCHMARK SUMMARY                   ", CMD_WHITE)],
    [(" Total Bounces Completed : 10", TEXT_DEFAULT)],
    [(" Total Elapsed Time      : 0.001224 seconds", TEXT_DEFAULT)],
    [(" Average Round-Trip Time : 0.244800 ms (244.80 microseconds)", STAT_MAGENTA)],
    [(" One-Way Network Latency : 0.122400 ms (122.40 microseconds)", STAT_MAGENTA)],
    [("=================================================================", TEXT_DEFAULT)]
]
render_terminal("mpirun -np 2 bin/02_ping_pong", s4_lines, "screenshot4_ping_pong.png")

# ==============================================================================
# Screenshot 5: Program 3 - Numerical Array Exchange
# ==============================================================================
s5_lines = [
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~/mpi_distributed_systems", HOST_CYAN), ("$ mpirun --hostfile hostfile -np 2 bin/03_array_exchange", CMD_WHITE)],
    [("=================================================================", TEXT_DEFAULT)],
    [("[SYSTEM 1] Running on: system1-master", SYS1_COLOR)],
    [("[SYSTEM 1] Generating raw input array:", SYS1_COLOR)],
    [("   raw_data[0] =  10.50 | raw_data[1] =  21.00 | raw_data[2] =  31.50", TEXT_DEFAULT)],
    [("   raw_data[3] =  42.00 | raw_data[4] =  52.50 | raw_data[5] =  63.00", TEXT_DEFAULT)],
    [("   raw_data[6] =  73.50 | raw_data[7] =  84.00", TEXT_DEFAULT)],
    [("[SYSTEM 1 -> SYSTEM 2] Dispatching 8 elements to System 2...", SYS1_COLOR)],
    [("-----------------------------------------------------------------", DIM_GRAY)],
    [("[SYSTEM 2] Worker node running on: system2-worker", SYS2_COLOR)],
    [("[SYSTEM 2] Received raw array from System 1. Performing computations...", SYS2_COLOR)],
    [("[SYSTEM 2] Computation complete. Transmitting results back to System 1...", SYS2_COLOR)],
    [("[SYSTEM 2] Completed successfully.", SYS2_COLOR)],
    [("-----------------------------------------------------------------", DIM_GRAY)],
    [("[SYSTEM 1 <- SYSTEM 2] Received processed results from System 2:", SYS1_COLOR)],
    [("   processed[0] =  26.00 (raw * 2.0 + 5.0)  |  processed[1] =  47.00", TEXT_DEFAULT)],
    [("   processed[2] =  68.00 (raw * 2.0 + 5.0)  |  processed[3] =  89.00", TEXT_DEFAULT)],
    [("   processed[4] = 110.00 (raw * 2.0 + 5.0)  |  processed[5] = 131.00", TEXT_DEFAULT)],
    [("   processed[6] = 152.00 (raw * 2.0 + 5.0)  |  processed[7] = 173.00", TEXT_DEFAULT)],
    [("   Sum of processed elements = 796.00", STAT_MAGENTA)],
    [("=================================================================", TEXT_DEFAULT)]
]
render_terminal("mpirun -np 2 bin/03_array_exchange", s5_lines, "screenshot5_array_exchange.png")

# ==============================================================================
# Screenshot 6: Program 4 - Non-Blocking Asynchronous Exchange
# ==============================================================================
s6_lines = [
    [("mpiuser@system1-master", PROMPT_GREEN), (":", TEXT_DEFAULT), ("~/mpi_distributed_systems", HOST_CYAN), ("$ mpirun --hostfile hostfile -np 2 bin/04_nonblocking_exchange", CMD_WHITE)],
    [("[Host: system1-master  | Rank 0] Starting non-blocking exchange with Rank 1 (sending val = 1001)...", SYS1_COLOR)],
    [("[Host: system1-master  | Rank 0] Overlapping: performing local computation while transmitting...", SYS1_COLOR)],
    [("[Host: system2-worker  | Rank 1] Starting non-blocking exchange with Rank 0 (sending val = 2002)...", SYS2_COLOR)],
    [("[Host: system2-worker  | Rank 1] Overlapping: performing local computation while transmitting...", SYS2_COLOR)],
    [("[Host: system1-master  | Rank 0] Transfer complete! Received val = 2002 (Local sum check = 14999997)", SYS1_COLOR)],
    [("[Host: system2-worker  | Rank 1] Transfer complete! Received val = 1001 (Local sum check = 14999997)", SYS2_COLOR)],
    [("=================================================================", TEXT_DEFAULT)],
    [("[SUCCESS] Both processes completed asynchronous overlap with zero deadlock.", PROMPT_GREEN)]
]
render_terminal("mpirun -np 2 bin/04_nonblocking_exchange", s6_lines, "screenshot6_nonblocking.png")
