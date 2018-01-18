#!/bin/bash
if [ $1 -z ]; then
  file=1k/01.bin
else
  file=$1
fi
filename=$(basename $file)
path=test/files/compression/random

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color


make
./viz -c -f $filename.viz $path/$file
./viz -d $filename.viz
echo -e "\n"
echo "Original file:"
hexdump -C $path/$file | tail
echo -e "\n"
echo "Extracted file:"
hexdump -C $filename | tail
echo -e "\n"
echo "VIZ file:"
hexdump -C $filename.viz | tail

sum1=$(sha256sum $path/$file | awk -F ' ' '{print $1}')
sum2=$(sha256sum $filename | awk -F ' ' '{print $1}')

if [ "$sum1" != "$sum2" ]; then
  echo -e "${RED}The files are different!$NC"
  exit 1
else
  echo -e "${GREEN}The files are the same.$NC"
fi
