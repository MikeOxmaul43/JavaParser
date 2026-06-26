#!/bin/bash
PARSER="../../build/KNPO_Java_Parser"
OUT_DIR="./outputs"
LOG_DIR="./logs"
mkdir -p "$OUT_DIR"
mkdir -p "$LOG_DIR"

# Основные тесты T*.java
for file in ./inputs/T*.java; do
    name=$(basename "$file" .java)
    echo "Running $name..."
    "$PARSER" "$file" "${OUT_DIR}/${name}"
done

# T27 — нет аргументов
echo "T27_NoArgs"
"$PARSER" 2>"${LOG_DIR}/T27_NoArgs.log"
echo "Exit code: $?"

# T28 — только входной файл
echo "T28_OnlyInput"
"$PARSER" ./inputs/T1.java 2>"${LOG_DIR}/T28_OnlyInput.log"
echo "Exit code: $?"

# T29 — лишний аргумент
echo "T29_ExtraArgs"
"$PARSER" ./inputs/T1.java ./outputs/T1 extra 2>"${LOG_DIR}/T29_ExtraArgs.log"
echo "Exit code: $?"

# T30 — несуществующий файл
echo "T30_MissingFile"
"$PARSER" ./inputs/DOES_NOT_EXIST.java ./outputs/test 2>"${LOG_DIR}/T30_MissingFile.log"
echo "Exit code: $?"

# T31 — пустой файл
echo "T31_EmptyFile"
touch ./inputs/empty.java
"$PARSER" ./inputs/empty.java ./outputs/test 2>"${LOG_DIR}/T31_EmptyFile.log"
echo "Exit code: $?"

# T32 — файл без прав на чтение
echo "T32_NoPermissionFile"
touch ./inputs/noperm.java
chmod 000 ./inputs/noperm.java
"$PARSER" ./inputs/noperm.java ./outputs/test 2>"${LOG_DIR}/T32_NoPermissionFile.log"
echo "Exit code: $?"
chmod 644 ./inputs/noperm.java

echo "Done."