#!/bin/bash
SHELL=./mysh
PASS=0; FAIL=0

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

run_test "echo simple"     "echo hello"       "hello"
run_test "echo multiple"   "echo hello world" "hello world"
run_test "unknown command" "fakecommand123"   "fakecommand123: No such file or directory"
run_test "empty input"     ""                 ""

actual=$(printf 'help' | $SHELL 2>&1 | head -1)
if [ "$actual" = "MyShell — Simple Unix Shell" ]; then
    echo "PASS: help first line"
    PASS=$((PASS + 1))
else
    echo "FAIL: help first line — got: $actual"
    FAIL=$((FAIL + 1))
fi

echo ""
echo "Results: $PASS passed, $FAIL failed"
