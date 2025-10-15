#!/bin/bash

# Simple wrapper to run minishell with ulimit restrictions
# This causes fork failures to test FD leak handling
#
# Usage:
#   ./ulimit_test.sh
#   ./ulimit_test.sh "cat file | grep test"
#
# Check FDs while running (in another terminal):
#   ls -la /proc/$(pgrep minishell)/fd

# Get current process count for this user
CURRENT_PROCS=$(ps -u $USER | wc -l)

# Add larger buffer for minishell and its children (30 more processes)
# This ensures the script can run, but large pipelines may still fail
ULIMIT_PROCS=$((CURRENT_PROCS + 30))

echo "Current processes: $CURRENT_PROCS"
echo "Setting ulimit -u to: $ULIMIT_PROCS (current + 30)"
echo ""

# Set limits and run
ulimit -u $ULIMIT_PROCS -n 40

if [ -n "$1" ]; then
    echo "$1" | exec ./minishell
else
    exec ./minishell
fi
