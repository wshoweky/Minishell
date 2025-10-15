#!/bin/bash

# Demonstration: See minishell processes in action
# This shows that minishell runs too fast to see with pgrep normally

echo "=========================================="
echo "DEMONSTRATION: Minishell Process Lifecycle"
echo "=========================================="
echo

# Auto-detect minishell location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
if [ -f "$SCRIPT_DIR/minishell" ]; then
    MINISHELL="$SCRIPT_DIR/minishell"
elif [ -f "$SCRIPT_DIR/../../minishell" ]; then
    MINISHELL="$SCRIPT_DIR/../../minishell"
elif [ -f "./minishell" ]; then
    MINISHELL="./minishell"
else
    MINISHELL="./minishell"
fi

if [ ! -f "$MINISHELL" ]; then
    echo "Error: minishell not found at: $MINISHELL"
    echo "Looking in: $SCRIPT_DIR"
    exit 1
fi

echo "Using minishell at: $MINISHELL"
echo

echo "1. Creating a test input file..."
echo "hello world" > /tmp/demo_test.txt
echo

echo "2. Running minishell with a quick command..."
echo "   Command: cat /tmp/demo_test.txt | grep hello"
echo

echo "3. Watching for minishell process (in background)..."
echo "   Press Ctrl+C to stop watching"
echo

# Start watching for minishell processes in background
(
    while true; do
        pids=$(pgrep -f "minishell" 2>/dev/null)
        if [ -n "$pids" ]; then
            echo "   [$(date +%H:%M:%S.%N | cut -b1-12)] ✓ Minishell running! PID(s): $pids"
        fi
        sleep 0.001  # Check every millisecond
    done
) &
WATCHER_PID=$!

# Give watcher time to start
sleep 0.1

echo "4. Executing minishell now..."
time (echo "cat /tmp/demo_test.txt | grep hello" | $MINISHELL > /dev/null 2>&1)
echo

# Let watcher run a bit more
sleep 0.5

# Stop watcher
kill $WATCHER_PID 2>/dev/null
wait $WATCHER_PID 2>/dev/null

echo
echo "5. Checking for minishell processes NOW..."
pids=$(pgrep -f "minishell" 2>/dev/null)
if [ -n "$pids" ]; then
    echo "   ✓ Found: $pids"
    ps -fp $pids
else
    echo "   ✗ No minishell processes found (they already exited)"
fi

echo
echo "=========================================="
echo "EXPLANATION:"
echo "=========================================="
echo "Minishell runs VERY quickly (typically 10-50ms)."
echo "By the time you run 'pgrep', it's already finished."
echo "The test script checks FDs BEFORE and AFTER, not DURING."
echo
echo "To see minishell running, you'd need:"
echo "  - A long-running command (like 'sleep 10')"
echo "  - Or run it in background with '&'"
echo "  - Or use very fast monitoring (like strace/perf)"
echo

rm -f /tmp/demo_test.txt

# Now demonstrate with a long-running command
echo "=========================================="
echo "BONUS: Long-running minishell"
echo "=========================================="
echo
echo "Running: echo 'sleep 3' | minishell &"
echo "Now you can see it with pgrep!"
echo

echo "sleep 3" | $MINISHELL &
MINISHELL_PID=$!

sleep 0.2

echo "Checking for minishell:"
pgrep -f "minishell" || echo "Not found yet..."
ps aux | grep -E "[m]inishell|[s]leep 3" || true

echo
echo "Waiting for it to finish..."
wait $MINISHELL_PID 2>/dev/null

echo "Done!"
