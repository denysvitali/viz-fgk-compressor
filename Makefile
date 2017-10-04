GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
CFLAGS = "-DGIT_VERSION=\"$(GIT_VERSION)\""

main:
	$(CC) $(CFLAGS) main.c -o viz