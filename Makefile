CC = mpicc
CFLAGS = -Wall -O2
LDFLAGS = -lm

SRC = src
BIN = bin

TARGETS = $(BIN)/01_basic_exchange \
          $(BIN)/02_ping_pong \
          $(BIN)/03_array_exchange \
          $(BIN)/04_nonblocking_exchange

all: $(BIN) $(TARGETS)

$(BIN):
	mkdir -p $(BIN)

$(BIN)/%: $(SRC)/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(BIN)

.PHONY: all clean
