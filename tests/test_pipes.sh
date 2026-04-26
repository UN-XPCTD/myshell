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

run_test "simple pipe"      "echo hello | cat"            "hello"
run_test "pipe with grep"   "echo hello | grep hello"     "hello"
run_test "pipe no match"    "echo hello | grep zzz"       ""
run_test "three stage pipe" "echo hello | cat | cat"      "hello"
run_test "ls pipe grep"     "ls src | grep signals.c"     "signals.c"

echo ""
echo "Results: $PASS passed, $FAIL failed"
