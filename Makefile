CC := gcc

PROFILER := valgrind
PROFILER_FLAGS := -v
PROFILER_FLAGS := $(PPROFILER_FLAGS) --error-exitcode=1

GIT_VERSION := $(shell git describe --abbrev=7 --dirty --always --tags)
CFLAGS = "-DGIT_VERSION=\"$(GIT_VERSION)\""
CFLAGS := $(CFLAGS) -g
CFLAGS := $(CFLAGS) -Wall
CFLAGS := $(CFLAGS) -lm
PRJ_FILES := main.c console.c fgk/*.c
TESTFLAGS = "-DTEST=1"

ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

main:
	$(CC) $(CFLAGS) $(PRJ_FILES) -o viz
test:
	echo "Detected OS $(detected_OS)"
	$(CC) $(CFLAGS) $(TESTFLAGS) $(PRJ_FILES) -o viz-test && ./viz-test

 ifneq ($(detected_OS),Windows)
	#$(PROFILER) $(PROFILER_FLAGS) ./viz-test
 endif

massif: test
	$(PROFILER) --tool=massif --massif-out-file=viz-test.massif ./viz-test
	massif-visualizer viz-test.massif

.PHONY: test
