#!/bin/bash
SHELL=./mysh
PASS=0; FAIL=0
TMP=$(mktemp -d)

run_test() {
    local desc="$1"
    local input="$2"
    local expected="$3"
    local actual
    actual=$(printf '%s' "$input" | $SHELL 2>&1)
    if [ "$actual" = "$expected" ]; then
        echo "PASS: $desc"
        PASS=$((PASS + 1))
    else
        echo "FAIL: $desc"
        echo "  expected: $(echo "$expected" | cat -A)"
        echo "  actual:   $(echo "$actual"   | cat -A)"
        FAIL=$((FAIL + 1))
    fi
}

run_test "output redirect" \
    "echo hello > $TMP/out.txt
cat $TMP/out.txt" \
    "hello"

run_test "append redirect" \
    "echo line1 > $TMP/app.txt
echo line2 >> $TMP/app.txt
cat $TMP/app.txt" \
    "line1
line2"

echo hello > $TMP/in.txt
run_test "input redirect" "cat < $TMP/in.txt" "hello"

run_test "missing input file" \
    "cat < /nonexistent_xyz" \
    "/nonexistent_xyz: No such file or directory"

rm -rf "$TMP"
echo ""
echo "Results: $PASS passed, $FAIL failed"
