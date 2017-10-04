CC := gcc
GIT_VERSION := $(shell git describe --abbrev=7 --dirty --always --tags)
CFLAGS = "-DGIT_VERSION=\"$(GIT_VERSION)\""

main:
	$(CC) $(CFLAGS) main.c -o viz
