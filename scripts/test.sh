#!/bin/bash
if [ -z "$1" ]; then
  echo "No file provided. Testing w/ the default one"
  file=test/files/compression/random/1k/01.bin
else
  file=$1
fi

binary=viz-release
filename=$(basename "$file")

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color


make release
./$binary -c -f "$filename.viz" "$file"
./$binary -d "$filename.viz"
echo -e "\n"
echo "Original file:"
hexdump -C "$file" | tail
echo -e "\n"
echo "Extracted file:"
hexdump -C "$filename" | tail
echo -e "\n"
echo "VIZ file:"
hexdump -C "$filename.viz" | tail

sum1=$(sha256sum "$file" | awk -F ' ' '{print $1}')
sum2=$(sha256sum "$filename" | awk -F ' ' '{print $1}')

rm "$filename"
rm "$filename.viz"

if [ "$sum1" != "$sum2" ]; then
  echo -e "${RED}The files are different!$NC"
  exit 1
else
  echo -e "${GREEN}The files are the same.$NC"
fi
