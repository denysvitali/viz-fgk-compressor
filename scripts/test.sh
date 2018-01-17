make && ./viz -c -f 1.viz test/files/compression/random/1k/01.bin && ./viz -d 1.viz && hexdump -C test/files/compression/random/1k/01.bin | tail && hexdump -C 01.bin | tail && hexdump -C 1.viz | tail
