#!/bin/bash
# Call me from the project root (./scripts/test-all-files.sh)
find test/files/compression -type f|while read i; do ./scripts/test.sh "$i"; done;
