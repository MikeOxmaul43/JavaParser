#!/bin/bash

PARSER="../../build/KNPO_Java_Parser"
OUT_DIR="./outputs"

mkdir -p "$OUT_DIR"

for file in ./inputs/T*.java; do
    name=$(basename "$file" .java)

    echo "Running $name..."

    "$PARSER" "$file" "${OUT_DIR}/${name}"
done

echo "Done."