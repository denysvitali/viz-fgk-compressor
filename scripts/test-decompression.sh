#!/bin/bash
for i in $(echo {02..10});
do 
    size=1k
    binary=viz-release
    ./$binary -c -f test1.viz test/files/compression/random/$size/$i.bin && ./$binary -d test1.viz
    sum1=$(sha256sum $i.bin | awk -F ' ' '{print $1}'); 
    sum2=$(sha256sum test/files/compression/random/$size/$i.bin | awk -F ' ' '{print $1}')
    if [ "$sum1" != "$sum2" ]; then 
        echo "ERROR, $sum1 != $sum2";
        echo "File was $size/$i.bin"
        exit 1;
    fi
done