# algoritmi-fgk-compression
![Build Status](https://jenkins.mastodonti.co/job/dvitali-algoritmi-fgk-compression/badge/icon)  
A C compressor, based on Adaptive Huffman Coding (FGK)

## Compilation
VIZ can be compiled with the following command:  
```
make
```
The output will be a `viz` binary, that can be called with `./viz`.

## Compression
Compression is performed with `viz -c output-file.viz input-file`, where `input-file` can be either a file or a directory.

## Decompression
You can decompress a `.viz` file with `viz -d input-file.viz`.


## Usage
```
VIZ compressor v0.0.1 (dad96c6-dirty)
Compress:        viz -c output.viz inputfile
Extract:         viz -d input.viz
```
## Resources
[Adaptive Huffman Coding - FGK - Stringology.org](http://www.stringology.org/DataCompression/fgk/index_en.html)  
[Adaptive Huffman Coding - Wikipedia](https://en.wikipedia.org/wiki/Adaptive_Huffman_coding)  
[Adaptive Huffman Coding - The Data Compression Guide](https://sites.google.com/site/datacompressionguide/fgk)  
[Adaptive Huffman Coding - cs.duke.edu](https://www.cs.duke.edu/csed/curious/compression/adaptivehuff.html)