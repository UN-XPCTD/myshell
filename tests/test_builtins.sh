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

run_test "cd and pwd"        "cd /tmp
pwd"                         "/tmp"

run_test "cd no arg goes home" "cd
pwd"                           "$HOME"

run_test "cd bad dir" \
    "cd /nonexistent_dir_xyz" \
    "cd: No such file or directory"

cat > /tmp/mysh_batch_test.sh << 'BEOF'
echo batch line one
echo batch line two
BEOF
actual=$($SHELL /tmp/mysh_batch_test.sh 2>&1)
expected="batch line one
batch line two"
if [ "$actual" = "$expected" ]; then
    echo "PASS: batch mode"
    PASS=$((PASS + 1))
else
    echo "FAIL: batch mode"
    echo "  expected: $expected"
    echo "  actual:   $actual"
    FAIL=$((FAIL + 1))
fi
rm -f /tmp/mysh_batch_test.sh

echo ""
echo "Results: $PASS passed, $FAIL failed"
