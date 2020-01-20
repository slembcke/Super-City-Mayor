CFLAGS += -std=c99 -Wall -Wextra -Os
BIN = png2chr chr2png lz4x png2edt

.phony: all clean

all: $(BIN)

clean:
	-rm -r *.o $(BIN)

slib.o: slib.h

png2chr: png2chr.o slib.o shared.o
	$(CC) -o $@ $^ $(CFLAGS) -lpng

chr2png: chr2png.o slib.o
	$(CC) -o $@ $^ $(CFLAGS) -lpng

png2tiles: png2tiles.c slib.o shared.o
	$(CC) -o $@ $^ $(CFLAGS) -lpng

lz4x: lz4x.cpp
	$(CC) -o $@ $<

png2edt: png2edt.o slib.o shared.o
	$(CC) -o $@ $^ $(CFLAGS) -lpng -lm
