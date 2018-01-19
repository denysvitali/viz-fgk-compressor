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
DEBUG_FLAGS := $(DEBUG_FLAGS)
RELEASE_FLAGS = -DDEBUG=0
RELEASE_FLAGS := $(RELEASE_FLAGS) -DRELEASE=1 -O3

PROFILER_COMPILER_FLAGS = -pg

DEBUG_ARGS = -c -f out.viz test/files/compression/text/fitnessgram.txt
DEBUG_ARGS_ALICE = -c -f out.viz test/files/compression/provided/alice.txt
RELEASE_ARGS = $(DEBUG_ARGS)
RELEASE_ARGS_ALICE = $(DEBUG_ARGS_ALICE)

COMPRESSION_ARGS = -c -f out.viz test/files/compression/provided/alice.txt
COMPRESSION_INTENSE_ARGS = -c -f out.viz "test/files/compression/img/nyny-denvit.jpg"
DECOMPRESSION_ARGS = -d test/files/decompression/alice.viz
DECOMPRESSION_INTENSE_ARGS = -d test/files/decompression/denvit-picture-1.viz

BENCHMARK_ARGS = -ao benchmark_results.txt -f "Took: %E"

.PHONY: clean
.PHONY: docs
.PHONY:	debug

all: clean debug

ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

debug:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(PRJ_FILES) -o viz

debug_profiler:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(PRJ_FILES) $(PROFILER_COMPILER_FLAGS) -o viz-profiler-debug

release:
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(PRJ_FILES) -o viz-release

release_profiler:
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(PRJ_FILES) $(PROFILER_COMPILER_FLAGS) -o viz-profiler-release

test: clean
	echo "Detected OS $(detected_OS)"
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(PRJ_FILES) -o viz-test && ./viz-test

 ifneq ($(detected_OS),Windows)
	#$(PROFILER) $(PROFILER_FLAGS) ./viz-test
 endif

test_profiler: clean
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(PRJ_FILES) -o viz-profiler-test

############## MASSIF ###################

massif: debug
	$(PROFILER) --tool=massif --massif-out-file=viz.massif ./viz $(DEBUG_ARGS)
	massif-visualizer viz.massif
massif_test: test
	$(PROFILER) --tool=massif --massif-out-file=viz-test.massif ./viz-test
	massif-visualizer viz-test.massif

massif_release: massif_release_c

massif_release_c: release
	$(PROFILER) --tool=massif --massif-out-file=viz-release.massif ./viz-release $(COMPRESSION_ARGS)
	massif-visualizer viz-release.massif
massif_release_d: release
	$(PROFILER) --tool=massif --massif-out-file=viz-release.massif ./viz-release $(DECOMPRESSION_ARGS)
	massif-visualizer viz-release.massif

massif_release_intense_c: release
	$(PROFILER) --tool=massif --massif-out-file=viz-release.massif ./viz-release $(COMPRESSION_INTENSE_ARGS)
	massif-visualizer viz-release.massif

massif_release_intense_d: release
	$(PROFILER) --tool=massif --massif-out-file=viz-release.massif ./viz-release $(DECOMPRESSION_INTENSE_ARGS)
	massif-visualizer viz-release.massif

############# VALGRIND #################

valgrind: valgrind_c
	$(PROFILER) --leak-check=full -v ./viz $(DEBUG_ARGS)

valgrind_c: debug
	$(PROFILER) --leak-check=full -v ./viz $(COMPRESSION_ARGS)

valgrind_d: debug
	$(PROFILER) --leak-check=full -v ./viz $(DECOMPRESSION_ARGS)

valgrind_test: test
	$(PROFILER) --leak-check=full -v ./viz-test

valgrind_release_c: release
	$(PROFILER) --leak-check=full -v ./viz-release $(COMPRESSION_ARGS)

valgrind_release_d: release
	$(PROFILER) --leak-check=full -v ./viz-release $(DECOMPRESSION_ARGS)

############ CALLGRIND #################

callgrind: callgrind_c

callgrind_c: debug
	$(PROFILER) --tool=callgrind --callgrind-out-file=viz-debug.callgrind ./viz $(COMPRESSION_ARGS)
	python utilities/gprof2dot/gprof2dot.py -f callgrind viz-debug.callgrind > viz-debug.callgrind.dot && xdot viz-debug.callgrind.dot

callgrind_d: debug
	$(PROFILER) --tool=callgrind --callgrind-out-file=viz-debug.callgrind ./viz $(DECOMPRESSION_ARGS)
	python utilities/gprof2dot/gprof2dot.py -f callgrind viz-debug.callgrind > viz-debug.callgrind.dot && xdot viz-debug.callgrind.dot

callgrind_test: test
	$(PROFILER) --tool=callgrind --callgrind-out-file=viz-test.callgrind ./viz-test $(DEBUG_ARGS)
	python utilities/gprof2dot/gprof2dot.py -f callgrind viz-test.callgrind > viz-test.callgrind.dot && xdot viz-test.callgrind.dot

callgrind_release: callgrind_release_c
callgrind_release_c: release
	$(PROFILER) --tool=callgrind --callgrind-out-file=viz-release.callgrind ./viz-release $(COMPRESSION_ARGS)
	python utilities/gprof2dot/gprof2dot.py -f callgrind viz-release.callgrind > viz-release.callgrind.dot && xdot viz-release.callgrind.dot

callgrind_release_d: release
	$(PROFILER) --tool=callgrind --callgrind-out-file=viz-release.callgrind ./viz-release $(DECOMPRESSION_ARGS)
	python utilities/gprof2dot/gprof2dot.py -f callgrind viz-release.callgrind > viz-release.callgrind.dot && xdot viz-release.callgrind.dot

callgrind_alice: debug
	$(PROFILER) --tool=callgrind --callgrind-out-file=viz-debug.callgrind ./viz $(DEBUG_ARGS_ALICE)
	python utilities/gprof2dot/gprof2dot.py -f callgrind viz-debug.callgrind > viz-debug.callgrind.dot && xdot viz-debug.callgrind.dot

callgrind_alice_release: release
	$(PROFILER) --tool=callgrind --callgrind-out-file=viz-release.callgrind ./viz-release $(RELEASE_ARGS_ALICE)
	python utilities/gprof2dot/gprof2dot.py -f callgrind viz-release.callgrind > viz-release.callgrind.dot && xdot viz-release.callgrind.dot

############### BENCHMARK ################
benchmark: release
	rm -f benchmark/result.txt
	#./scripts/benchmark.sh test/files/provided/immagine.tiff
	IFS=$$'\n'; for i in $$(find test/files/compression/ -type f); do ./scripts/benchmark.sh "$${i}"; done
	#./scripts/benchmark.sh test/files/provided/alice.txt
	#./scripts/benchmark.sh test/files/text/bible.txt
	#./scripts/benchmark.sh test/files/text/fitnessgram.txt
	#./scripts/benchmark.sh test/files/text/adaptivehuffman.txt
	#./scripts/benchmark.sh test/files/text/a.txt
	#for i in {01..10}; do ./scripts/benchmark.sh test/files/random/1M/$$i.bin; done

############### DOCS ################
docs:
	pandoc -s docs/metadata.yaml --toc docs/README.md --resource-path=docs/ -o docs/documentation.pdf
	pandoc -s docs/metadata.yaml README.md -s -o docs/readme.pdf

clean:
	rm -f {viz,viz-test,viz-release,viz-profiler*} *.callgrind vgcore.* *.massif *.viz *.dot *.tmp callgrind.out.* gmon.out *.callgrind.dot
