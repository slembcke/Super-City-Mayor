.phony: all clean

all: text2data nsf2data

clean:
	-rm text2data nsf2data

text2data: text2data.cpp
	$(CC) -o $@ $<

nsf2data: nsf2data.cpp
	$(CC) -o $@ $<
