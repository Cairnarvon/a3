CC=gcc
DEFS=-D_BSD_SOURCE -D_POSIX_C_SOURCE=2
WARNS=-Wall -Wextra -pedantic -ansi
CFLAGS:=$(CFLAGS) $(DEFS) $(WARNS) -lz

.PHONY: all
all: a3show a3make

a3show: a3show.c a3.c
	$(CC) -o $@ $(CFLAGS) $^

a3make: a3make.c a3.c
	$(CC) -o $@ $(CFLAGS) $^

.PHONY: clean
clean:
	rm -f a3show a3make
