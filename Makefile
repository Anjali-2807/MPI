# ==============================================================================
# Makefile for MPI Two-System Data Exchange Programs
# ==============================================================================

CC = mpicc
CFLAGS = -Wall -O2
LDFLAGS = -lm

BIN_DIR = bin
SRC_DIR = src

TARGETS = $(BIN_DIR)/01_basic_exchange \
          $(BIN_DIR)/02_ping_pong \
          $(BIN_DIR)/03_array_exchange \
          $(BIN_DIR)/04_nonblocking_exchange

all: $(BIN_DIR) $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/01_basic_exchange: $(SRC_DIR)/01_basic_exchange.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_DIR)/02_ping_pong: $(SRC_DIR)/02_ping_pong.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_DIR)/03_array_exchange: $(SRC_DIR)/03_array_exchange.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_DIR)/04_nonblocking_exchange: $(SRC_DIR)/04_nonblocking_exchange.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean

