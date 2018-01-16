#!/bin/bash

PROJECT_DIR="$(dirname $0)/.."
RESULT_DIR="$PROJECT_DIR/benchmark"
OUTPUT_DIR="$RESULT_DIR/files"
RESULT_FILE="$RESULT_DIR/result.txt"


function benchmark(){
  rm $RESULT_DIR/benchmark.txt

  FILE_NAME=$(basename $1)
  echo "File Name: $FILE_NAME"

  TIME_ARGS="%E"

  ORIGINAL_FILE_SIZE=$(stat -c '%s' $1)
  /bin/time -o $RESULT_DIR/benchmark.txt -f $TIME_ARGS $PROJECT_DIR/viz-release -c -f $OUTPUT_DIR/$FILE_NAME.viz $PROJECT_DIR/$1
  ELAPSED_C=$(cat $RESULT_DIR/benchmark.txt)
  /bin/time -o $RESULT_DIR/benchmark.txt -f $TIME_ARGS $PROJECT_DIR/viz-release -d $OUTPUT_DIR/$FILE_NAME.viz
  ELAPSED_D=$(cat $RESULT_DIR/benchmark.txt)
  COMPRESSED_SIZE=$(stat -c '%s' $OUTPUT_DIR/$FILE_NAME.viz)
  echo "Original File Size: $ORIGINAL_FILE_SIZE"
  echo "Compressed File Size: $COMPRESSED_SIZE"
  COMPRESSION_RATE=$(bc <<< "scale=4;(1-($COMPRESSED_SIZE/$ORIGINAL_FILE_SIZE))*100")
  echo "Compression rate: $COMPRESSION_RATE"

  TIME_RESULT=$(cat $RESULT_DIR/benchmark.txt)
  echo >> $RESULT_DIR/result.txt
  echo "=====" >> $RESULT_DIR/result.txt
  echo "Name: $FILE_NAME" >> $RESULT_DIR/result.txt
  echo "Original Size: $ORIGINAL_FILE_SIZE" >> $RESULT_DIR/result.txt
  echo "Compressed Size: $COMPRESSED_SIZE" >> $RESULT_DIR/result.txt
  echo "Compression Rate: $COMPRESSION_RATE" >> $RESULT_DIR/result.txt
  echo "Elapsed time (C): $ELAPSED_C" >> $RESULT_DIR/result.txt
  echo "Elapsed time (D): $ELAPSED_D" >> $RESULT_DIR/result.txt
  echo "======" >> $RESULT_DIR/result.txt
}

function create_benchmark_dirs(){
   mkdir -p $OUTPUT_DIR
}

create_benchmark_dirs

benchmark $1

echo $PROJECT_DIR/viz-release -c $PROJECT_DIR/$1 $OUTPUT_DIR/
