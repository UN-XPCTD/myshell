#!/bin/bash
cd "$(dirname "$0")/.."

echo "============================="
echo " MyShell Test Suite"
echo "============================="
echo ""

echo "--- Basic Commands ---"
bash tests/test_basic.sh

echo ""
echo "--- I/O Redirection ---"
bash tests/test_redirection.sh

echo ""
echo "--- Piping ---"
bash tests/test_pipes.sh

echo ""
echo "--- Built-ins & Batch Mode ---"
bash tests/test_builtins.sh

echo ""
echo "============================="
echo " All tests complete"
echo "============================="
