#!/bin/bash

# ============================================================================
# MANUAL FD LEAK TESTING SCRIPT
# ============================================================================
# This script builds minishell and runs it with resource limits to test
# for file descriptor leaks. You can monitor FDs in real-time.
#
# Usage:
#   ./test_fd_leaks_manual.sh
#   ./test_fd_leaks_manual.sh --build-only    # Just build, don't run
#   ./test_fd_leaks_manual.sh --no-build      # Skip build, just run
# ============================================================================

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

BUILD=1
RUN=1

# Parse arguments
for arg in "$@"; do
    case $arg in
        --build-only) RUN=0 ;;
        --no-build)   BUILD=0 ;;
        --help)
            echo "Usage: $0 [--build-only] [--no-build] [--help]"
            echo "  --build-only  Build minishell but don't run the test"
            echo "  --no-build    Skip building, just run the test"
            echo "  --help        Show this help message"
            exit 0
            ;;
    esac
done

echo -e "${BOLD}${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BOLD}${BLUE}║${NC}  ${BOLD}Manual FD Leak Testing for Minishell${NC}"
echo -e "${BOLD}${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""

# ============================================================================
# STEP 1: BUILD MINISHELL
# ============================================================================

if [ $BUILD -eq 1 ]; then
    echo -e "${CYAN}▸ Step 1: Building minishell...${NC}"
    echo ""
    
    if [ ! -f "Makefile" ]; then
        echo -e "${RED}✗ Error: Makefile not found${NC}"
        echo -e "  Please run this script from the project root directory"
        exit 1
    fi
    
    echo -e "  Running: ${BOLD}make re${NC}"
    make re
    
    if [ ! -f "./minishell" ]; then
        echo -e "${RED}✗ Error: Build failed - minishell binary not found${NC}"
        exit 1
    fi
    
    echo ""
    echo -e "${GREEN}✓ Build successful!${NC}"
    echo ""
else
    echo -e "${YELLOW}▸ Skipping build (--no-build flag)${NC}"
    echo ""
fi

if [ $RUN -eq 0 ]; then
    echo -e "${GREEN}✓ Done (--build-only flag)${NC}"
    exit 0
fi

# ============================================================================
# STEP 2: CREATE TEST FILE
# ============================================================================

echo -e "${CYAN}▸ Step 2: Creating test file...${NC}"
echo ""

TEST_FILE="/tmp/minishell_test_file.txt"
echo "test data" > "$TEST_FILE"
echo "more test data" >> "$TEST_FILE"
echo "even more test" >> "$TEST_FILE"

echo -e "  Created: ${BOLD}$TEST_FILE${NC}"
echo ""

# ============================================================================
# STEP 3: EXPLAIN WHAT WILL HAPPEN
# ============================================================================

echo -e "${CYAN}▸ Step 3: Test Configuration${NC}"
echo ""
echo -e "  ${BOLD}Command to run:${NC}"
echo -e "  ${GREEN}ulimit -u 10; echo 'cat $TEST_FILE | grep x | sort | wc' | ./minishell${NC}"
echo ""
echo -e "  ${BOLD}What this does:${NC}"
echo -e "  • ${YELLOW}ulimit -u 10${NC}  - Limits process count to 10"
echo -e "  • This will likely cause ${RED}fork() failures${NC} in the pipeline"
echo -e "  • Minishell should ${GREEN}clean up FDs${NC} even when fork() fails"
echo ""
echo -e "  ${BOLD}What to watch for:${NC}"
echo -e "  • Check if FDs are leaked when fork() fails"
echo -e "  • Monitor /proc/PID/fd during execution"
echo ""

# ============================================================================
# STEP 4: RUN MINISHELL WITH MONITORING
# ============================================================================

echo -e "${CYAN}▸ Step 4: Running minishell with FD monitoring...${NC}"
echo ""

# Create a monitoring script
MONITOR_SCRIPT="/tmp/fd_monitor_$$.sh"
cat > "$MONITOR_SCRIPT" << 'MONITOR_EOF'
#!/bin/bash

# Find minishell PIDs
find_minishell_pids() {
    pgrep -f "^\./minishell" 2>/dev/null
}

# Monitor FDs for a PID
monitor_fds() {
    local pid=$1
    if [ -d "/proc/$pid/fd" ]; then
        local fd_count=$(ls /proc/$pid/fd 2>/dev/null | wc -l)
        echo "  [PID $pid] Open FDs: $fd_count"
        ls -la /proc/$pid/fd 2>/dev/null | tail -n +4 | sed 's/^/    /'
    fi
}

echo "  Monitoring for minishell processes..."
echo "  (This will show FDs in real-time)"
echo ""

# Monitor continuously for a few seconds
for i in {1..20}; do
    pids=$(find_minishell_pids)
    if [ -n "$pids" ]; then
        echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        echo "  Timestamp: $(date +%H:%M:%S.%N | cut -b1-12)"
        for pid in $pids; do
            monitor_fds $pid
        done
    fi
    sleep 0.1
done

echo ""
echo "  Monitoring complete."
MONITOR_EOF

chmod +x "$MONITOR_SCRIPT"

# Run monitoring in background
"$MONITOR_SCRIPT" &
MONITOR_PID=$!

# Give monitor time to start
sleep 0.2

# Run minishell with ulimit
echo -e "${YELLOW}  Running minishell with resource limits...${NC}"
echo ""
echo -e "${GREEN}  ┌─ MINISHELL OUTPUT ─────────────────────────────────┐${NC}"

(ulimit -u 10; echo "cat $TEST_FILE | grep x | sort | wc" | ./minishell) 2>&1 | sed 's/^/  │ /'

echo -e "${GREEN}  └────────────────────────────────────────────────────┘${NC}"
echo ""

# Wait for monitor to finish
sleep 1
kill $MONITOR_PID 2>/dev/null || true
wait $MONITOR_PID 2>/dev/null || true

# ============================================================================
# STEP 5: MANUAL CHECKING INSTRUCTIONS
# ============================================================================

echo ""
echo -e "${CYAN}▸ Step 5: How to Check for FD Leaks Manually${NC}"
echo ""
echo -e "  ${BOLD}If you want to check FDs while minishell is running:${NC}"
echo ""
echo -e "  ${YELLOW}1. In Terminal 1:${NC}"
echo -e "     ${GREEN}ulimit -u 10${NC}"
echo -e "     ${GREEN}echo 'sleep 5' | ./minishell &${NC}"
echo -e "     ${GREEN}MINISHELL_PID=\$!${NC}"
echo ""
echo -e "  ${YELLOW}2. In Terminal 2 (quickly!):${NC}"
echo -e "     ${GREEN}ls -la /proc/\$MINISHELL_PID/fd${NC}"
echo -e "     ${GREEN}# Or use the PID from Terminal 1${NC}"
echo ""
echo -e "  ${YELLOW}3. Alternative - continuous monitoring:${NC}"
echo -e "     ${GREEN}watch -n 0.1 'ls /proc/\$(pgrep minishell | head -1)/fd 2>/dev/null | wc -l'${NC}"
echo ""

# ============================================================================
# STEP 6: CHECK FOR LEAKED FDS NOW
# ============================================================================

echo -e "${CYAN}▸ Step 6: Checking for leaked FDs in current shell${NC}"
echo ""

BEFORE_FDS=$(ls /proc/$$/fd 2>/dev/null | wc -l)
echo -e "  File descriptors in this shell: ${BOLD}$BEFORE_FDS${NC}"

# Check if there are any lingering minishell processes
LINGERING=$(pgrep -f "^\./minishell" 2>/dev/null || echo "")
if [ -n "$LINGERING" ]; then
    echo -e "  ${YELLOW}⚠ Warning: Found lingering minishell processes!${NC}"
    for pid in $LINGERING; do
        fd_count=$(ls /proc/$pid/fd 2>/dev/null | wc -l || echo "0")
        echo -e "    PID $pid has $fd_count open FDs"
    done
    
    echo ""
    echo -e "  ${CYAN}To check their FDs:${NC}"
    for pid in $LINGERING; do
        echo -e "    ${GREEN}ls -la /proc/$pid/fd${NC}"
    done
else
    echo -e "  ${GREEN}✓ No lingering minishell processes${NC}"
fi

echo ""

# ============================================================================
# STEP 7: CLEANUP
# ============================================================================

echo -e "${CYAN}▸ Step 7: Cleanup${NC}"
echo ""

rm -f "$TEST_FILE" "$MONITOR_SCRIPT"
echo -e "  ${GREEN}✓ Cleaned up test files${NC}"

echo ""
echo -e "${BOLD}${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BOLD}${GREEN}║${NC}  ${BOLD}Test Complete!${NC}"
echo -e "${BOLD}${GREEN}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${CYAN}Summary:${NC}"
echo -e "  • Minishell was run with ${YELLOW}ulimit -u 10${NC}"
echo -e "  • This likely caused ${RED}fork() failures${NC} during pipeline execution"
echo -e "  • Check the output above for any ${RED}leaked FDs${NC}"
echo ""
echo -e "${CYAN}Next Steps:${NC}"
echo -e "  1. Review the FD monitoring output above"
echo -e "  2. If FDs were leaked, fix the fork failure path in pipeline.c"
echo -e "  3. Run this script again to verify the fix"
echo -e "  4. Use automated tests: ${GREEN}./src/playground/fd_leak_test.sh --quick${NC}"
echo ""
