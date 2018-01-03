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

TEST_FLAGS = -DTEST=1
TEST_FLAGS := $(TEST_FLAGS) -DDEBUG=0
DEBUG_FLAGS = -DDEBUG=1
RELEASE_FLAGS = -DDEBUG=0
RELEASE_FLAGS := $(RELEASE_FLAGS) -DRELEASE=1

ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

debug:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(PRJ_FILES) -o viz

main:
	clean
	debug

release:
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(PRJ_FILES) -o viz-release
test:
	echo "Detected OS $(detected_OS)"
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(PRJ_FILES) -o viz-test && ./viz-test

 ifneq ($(detected_OS),Windows)
	#$(PROFILER) $(PROFILER_FLAGS) ./viz-test
 endif

massif: test
	$(PROFILER) --tool=massif --massif-out-file=viz-test.massif ./viz-test
	massif-visualizer viz-test.massif
massif_prod: main
	$(PROFILER) --tool=massif --massif-out-file=viz.massif ./viz -c out.viz test/files/text/fitnessgram.txt
	massif-visualizer viz.massif
.PHONY: test
.PHONY: clean

clean:
	rm -f {viz,viz-test,viz-release} *.massif *.viz *.dot *.tmp
