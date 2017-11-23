CC := gcc
GIT_VERSION := $(shell git describe --abbrev=7 --dirty --always --tags)
CFLAGS = "-DGIT_VERSION=\"$(GIT_VERSION)\""
CFLAGS := $(CFLAGS) -g
CFLAGS := $(CFLAGS) -Wall
PRJ_FILES := main.c console.c fgk/*.c
TESTFLAGS = "-DTEST=1"

main:
	$(CC) $(CFLAGS) $(PRJ_FILES) -o viz
test:
	$(CC) $(CFLAGS) $(TESTFLAGS) $(PRJ_FILES) -o viz-test && ./viz-test

.PHONY: test