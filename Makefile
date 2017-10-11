CC := gcc
GIT_VERSION := $(shell git describe --abbrev=7 --dirty --always --tags)
CFLAGS = "-DGIT_VERSION=\"$(GIT_VERSION)\""
CFLAGS := -g
CFLAGS := $(CFLAGS) -Wall
TESTFLAGS = "-DTEST=1"

main:
	$(CC) $(CFLAGS) main.c -o viz
test:
	$(CC) $(CFLAGS) $(TESTFLAGS) main.c -o viz-test && ./viz-test

.PHONY: test