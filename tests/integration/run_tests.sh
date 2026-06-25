#!/bin/bash

PARSER="../../build-coverage/KNPO_Java_Parser"
OUT_DIR="./outputs"

mkdir -p "$OUT_DIR"

for file in ./inputs/T*.java; do
    name=$(basename "$file" .java)

    echo "Running $name..."

    "$PARSER" "$file" "${OUT_DIR}/${name}"
done

echo "Test: no args"
"$PARSER"

echo "Test: only input"
"$PARSER" ./inputs/T1.java

echo "Test: extra args"
"$PARSER" ./inputs/T1.java ./outputs/T1 extra

echo "Test: missing file"
"$PARSER" ./inputs/DOES_NOT_EXIST.java ./outputs/test

echo "Test: empty file"
touch ./inputs/empty.java
"$PARSER" ./inputs/empty.java ./outputs/test

echo "Test: no permission file"
touch ./inputs/noperm.java
chmod 000 ./inputs/noperm.java
"$PARSER" ./inputs/noperm.java ./outputs/test
chmod 644 ./inputs/noperm.java

echo "Done."
