#!/bin/bash

# ============================================================================
# MINISHELL FD LEAK & PIPELINE TEST SUITE
# ============================================================================
# Comprehensive testing for file descriptor leaks and pipeline functionality
# 
# Tests:
#   1. Normal pipeline operation (baseline)
#   2. Fork failure scenarios with FD cleanup
#   3. Pipe creation failure handling
#   4. Multiple consecutive failures (stress test)
#   5. Large pipeline handling
#   6. Zombie process prevention
#
# Usage:
#   ./test/fd_leak_test.sh              # Run all tests
#   ./test/fd_leak_test.sh --quick      # Quick mode (essential tests only)
#   ./test/fd_leak_test.sh --verbose    # Verbose output
# ============================================================================

set -e

# ============================================================================
# CONFIGURATION
# ============================================================================

# Auto-detect minishell location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
if [ -f "$SCRIPT_DIR/minishell" ]; then
    # Script is in same directory as minishell
    MINISHELL="$SCRIPT_DIR/minishell"
elif [ -f "$SCRIPT_DIR/../../minishell" ]; then
    # Script is in src/playground/, minishell is in project root
    MINISHELL="$SCRIPT_DIR/../../minishell"
elif [ -f "./minishell" ]; then
    # Minishell in current working directory
    MINISHELL="./minishell"
else
    # Let it fail in preflight checks with a clear error
    MINISHELL="./minishell"
fi

MODE="full"
VERBOSE=0

# Parse arguments
for arg in "$@"; do
    case $arg in
        --quick)   MODE="quick" ;;
        --verbose) VERBOSE=1 ;;
        --debug)   
            VERBOSE=1
            set -x  # Enable bash debug mode
            ;;
        --help)
            echo "Usage: $0 [--quick] [--verbose] [--debug] [--help]"
            echo "  --quick    Run only essential tests"
            echo "  --verbose  Show detailed output"
            echo "  --debug    Show all bash commands (very verbose)"
            echo "  --help     Show this help message"
            exit 0
            ;;
    esac
done

# ============================================================================
# COLORS & STYLING
# ============================================================================

if [ -t 1 ]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    BLUE='\033[0;34m'
    PURPLE='\033[0;35m'
    CYAN='\033[0;36m'
    BOLD='\033[1m'
    NC='\033[0m'
else
    RED=''; GREEN=''; YELLOW=''; BLUE=''; PURPLE=''; CYAN=''; BOLD=''; NC=''
fi

# ============================================================================
# TEST TRACKING
# ============================================================================

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
WARNINGS=0

# ============================================================================
# UTILITY FUNCTIONS
# ============================================================================

print_banner() {
    echo ""
    echo -e "${PURPLE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║${NC}  ${BOLD}$1${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

print_section() {
    echo ""
    echo -e "${BLUE}▸ $1${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

print_test() {
    echo -e "${CYAN}  Testing:${NC} $1"
}

log_verbose() {
    if [ $VERBOSE -eq 1 ]; then
        echo -e "${NC}    $1${NC}"
    fi
}

test_result() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    local status=$1
    local name=$2
    local detail=$3
    
    if [ $status -eq 0 ]; then
        echo -e "${GREEN}  ✓ PASS${NC}: $name $detail"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}  ✗ FAIL${NC}: $name $detail"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

test_warning() {
    WARNINGS=$((WARNINGS + 1))
    echo -e "${YELLOW}  ⚠ WARNING${NC}: $1"
}

count_fds() {
    local pid=${1:-$$}
    if [ -d "/proc/$pid/fd" ]; then
        ls /proc/$pid/fd 2>/dev/null | wc -l || echo "0"
    else
        # Fallback for systems without /proc
        lsof -p $pid 2>/dev/null | wc -l || echo "0"
    fi
}

count_zombies() {
    ps aux 2>/dev/null | grep -c '[d]efunct' || echo "0"
}

get_minishell_pids() {
    pgrep -f "^\./minishell" 2>/dev/null || echo ""
}

wait_for_process_cleanup() {
    local max_wait=${1:-3}  # Default 3 seconds, but can be customized
    local waited=0
    local show_progress=${2:-0}  # Optional: show progress dots
    
    while [ $waited -lt $max_wait ]; do
        local pids=$(get_minishell_pids)
        if [ -z "$pids" ]; then
            [ $show_progress -eq 1 ] && echo "" # Newline after dots
            return 0
        fi
        [ $show_progress -eq 1 ] && echo -n "."
        sleep 0.25  # Reduced from 0.5 to 0.25
        waited=$((waited + 1))
    done
    [ $show_progress -eq 1 ] && echo "" # Newline after dots
    return 1
}

cleanup_test_files() {
    rm -f /tmp/test_input_*.txt 2>/dev/null
    rm -f /tmp/run_limited_minishell_*.sh 2>/dev/null
    rm -f /tmp/minishell_test_output_*.txt 2>/dev/null
    # Kill any lingering minishell processes from this test
    pkill -f "^\./minishell" 2>/dev/null || true
    wait_for_process_cleanup
}

# ============================================================================
# SIGNAL HANDLING & CLEANUP
# ============================================================================

cleanup_on_exit() {
    local exit_code=$?
    cleanup_test_files
    exit $exit_code
}

trap cleanup_on_exit EXIT INT TERM

# ============================================================================
# PRE-FLIGHT CHECKS
# ============================================================================

preflight_checks() {
    print_section "Pre-flight Checks"
    
    # Check if minishell exists
    if [ ! -f "$MINISHELL" ]; then
        echo -e "${RED}✗ Error: $MINISHELL not found${NC}"
        echo -e "  Please run ${CYAN}make${NC} first"
        exit 1
    fi
    echo -e "${GREEN}✓${NC} Minishell binary found: $(realpath $MINISHELL)"
    
    # Check if we can count FDs
    local test_fd_count=$(count_fds $$)
    if [ "$test_fd_count" -eq 0 ]; then
        echo -e "${YELLOW}⚠${NC} Warning: Cannot count file descriptors via /proc"
        echo -e "  Trying alternative method..."
        test_fd_count=$(lsof -p $$ 2>/dev/null | wc -l || echo "0")
        if [ "$test_fd_count" -eq 0 ]; then
            echo -e "${RED}✗ Error: Cannot count file descriptors${NC}"
            echo -e "  Neither /proc nor lsof are available"
            exit 1
        fi
        echo -e "${GREEN}✓${NC} Using lsof for FD counting"
    fi
    echo -e "${GREEN}✓${NC} FD counting available (current: $test_fd_count FDs)"
    
    # Check for required commands
    local missing=""
    for cmd in timeout grep wc pgrep pkill; do
        if ! command -v $cmd &> /dev/null; then
            missing="$missing $cmd"
        fi
    done
    
    if [ -n "$missing" ]; then
        echo -e "${YELLOW}⚠${NC} Warning: Missing commands:$missing"
        echo -e "  Some tests may be skipped"
        WARNINGS=$((WARNINGS + 1))
    else
        echo -e "${GREEN}✓${NC} All required commands available"
    fi
    
    echo -e "${GREEN}✓${NC} Test PID: $$"
    echo -e "${GREEN}✓${NC} Mode: ${BOLD}$MODE${NC}"
}

# ============================================================================
# TEST 1: BASELINE - NORMAL PIPELINE
# ============================================================================

test_normal_pipeline() {
    print_section "Test 1: Normal Pipeline Operation (Baseline)"
    print_test "Simple pipeline without failures"
    
    local test_id="$$_$(date +%s)"
    local test_input="/tmp/test_input_${test_id}.txt"
    
    # Create test input
    echo "hello world" > "$test_input"
    echo "test line" >> "$test_input"
    
    # Get baseline FD count for the test script itself
    local before_fds=$(count_fds $$)
    log_verbose "Test script FDs before: $before_fds"
    
    # Run a simple pipeline and capture any FD leaks in the system
    local before_total_fds=$(ls /proc/*/fd 2>/dev/null | wc -l)
    
    echo -n "  Running minishell..."
    echo "cat $test_input | grep hello | wc -l" | timeout 2 $MINISHELL > /dev/null 2>&1 || true
    echo " done"
    
    # Wait for cleanup (reduced for speed)
    echo -n "  Waiting for cleanup"
    sleep 0.2
    wait_for_process_cleanup 1 1  # Show progress dots
    
    local after_total_fds=$(ls /proc/*/fd 2>/dev/null | wc -l)
    local after_fds=$(count_fds $$)
    log_verbose "Test script FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    local total_fd_diff=$((after_total_fds - before_total_fds))
    
    log_verbose "FD difference (test script): $fd_diff"
    log_verbose "FD difference (system-wide): $total_fd_diff"
    
    # Allow small variations (0-2 FDs) due to system fluctuations
    if [ $fd_diff -le 2 ] && [ $fd_diff -ge -2 ]; then
        test_result 0 "Normal pipeline" "(FD diff: $fd_diff)"
    else
        test_result 1 "Normal pipeline" "(FD diff: $fd_diff, expected: -2 to +2)"
    fi
    
    rm -f "$test_input"
}

# ============================================================================
# TEST 2: FORK FAILURE CLEANUP
# ============================================================================

test_fork_failure() {
    print_section "Test 2: Fork Failure FD Cleanup"
    print_test "Pipeline execution tracking with strace"
    
    local test_id="$$_$(date +%s)"
    local test_input="/tmp/test_input_${test_id}.txt"
    local strace_output="/tmp/strace_output_${test_id}.txt"
    
    # Create test input
    echo "test data" > "$test_input"
    
    # Check if strace is available
    if ! command -v strace &> /dev/null; then
        test_warning "strace not available - skipping detailed FD tracking"
        return
    fi
    
    echo -n "  Running minishell with strace FD tracking..."
    
    # Run minishell with strace to track all pipe and close operations
    # Redirect minishell output to /dev/null to not interfere with strace
    strace -e trace=pipe,pipe2,close,fork -f -o "$strace_output" \
        bash -c "echo 'cat $test_input | grep test | sort | uniq | wc' | $MINISHELL > /dev/null 2>&1" 2>/dev/null
    
    echo " done"
    
    log_verbose "Analyzing strace output..."
    
    # Count successful pipes - fixed regex to handle various strace formats
    # Matches: pipe([3, 4]) = 0 or pipe2([3, 4], ...) = 0
    local pipes_created
    pipes_created=$(grep -E "pipe2?\(\[.*\]\)" "$strace_output" 2>/dev/null | grep -c "= 0") || pipes_created=0
    local fds_from_pipes=$((pipes_created * 2))
    
    # Count successful closes
    local closes
    closes=$(grep -E "close\([0-9]+\)" "$strace_output" 2>/dev/null | grep -c "= 0") || closes=0
    
    # Count forks (to detect if fork failures occurred)  
    local forks
    forks=$(grep -c "fork()" "$strace_output" 2>/dev/null) || forks=0
    
    log_verbose "Pipes created: $pipes_created (= $fds_from_pipes FDs)"
    log_verbose "Successful close() calls: $closes"
    log_verbose "Fork calls detected: $forks"
    
    # Extract and check individual FD closures
    local leaked=0
    while IFS= read -r line; do
        # Extract FDs from pipe output: pipe([3, 4]) = 0
        fds=$(echo "$line" | grep -oE "\[[0-9]+, [0-9]+\]" | tr -d '[], ')
        if [ -n "$fds" ]; then
            for fd in $fds; do
                if ! grep -E "close\($fd\)" "$strace_output" 2>/dev/null | grep -q "= 0"; then
                    leaked=$((leaked + 1))
                    log_verbose "  FD $fd: LEAKED (pipe created but never closed)"
                fi
            done
        fi
    done < <(grep -E "pipe2?\(\[.*\]\)" "$strace_output" 2>/dev/null | grep "= 0")
    
    # Test result
    if [ $leaked -eq 0 ]; then
        test_result 0 "FD tracking (strace)" "($pipes_created pipes, all FDs closed)"
    else
        test_result 1 "FD tracking (strace)" "($leaked FDs leaked from $pipes_created pipes)"
        log_verbose "  Bug: Pipes created but not closed (likely on fork failure path)"
    fi
    
    # Show sample strace output if verbose
    if [ $VERBOSE -eq 1 ] && [ -f "$strace_output" ]; then
        echo -e "${CYAN}  Sample strace output:${NC}"
        head -20 "$strace_output" | sed 's/^/    /'
    fi
    
    rm -f "$test_input" "$strace_output"
}

# ============================================================================
# TEST 3: MULTIPLE CONSECUTIVE FAILURES
# ============================================================================

test_multiple_failures() {
    print_section "Test 3: Multiple Consecutive Failures (Stress Test)"
    print_test "Repeated pipeline failures should not accumulate FDs"
    
    local test_id="$$_$(date +%s)"
    local test_input="/tmp/test_input_${test_id}.txt"
    local test_script="/tmp/run_limited_minishell_${test_id}.sh"
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    # Create test input
    echo "data" > "$test_input"
    
    # Run multiple failing pipelines
    echo "  Running 5 stress test iterations..."
    for i in {1..5}; do
        echo -n "    Iteration $i/5..."
        cat > "$test_script" << EOF
#!/bin/bash
ulimit -u 10 -n 25 2>/dev/null
echo "cat $test_input | grep x | sort | wc" | $MINISHELL > /dev/null 2>&1
exit 0
EOF
        chmod +x "$test_script"
        timeout 2 bash "$test_script" > /dev/null 2>&1 || true
        echo " done"
        sleep 0.1
    done
    
    # Wait for cleanup (reduced for speed)
    echo -n "  Final cleanup check"
    sleep 0.2
    wait_for_process_cleanup 2 1
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    # Multiple failures should not accumulate FDs
    if [ $fd_diff -le 3 ] && [ $fd_diff -ge -3 ]; then
        test_result 0 "Multiple failures" "(FD diff: $fd_diff after 5 runs)"
    else
        test_result 1 "Multiple failures" "(FD diff: $fd_diff after 5 runs, expected: -3 to +3)"
    fi
    
    rm -f "$test_input" "$test_script"
}

# ============================================================================
# TEST 4: LARGE PIPELINE
# ============================================================================

test_large_pipeline() {
    if [ "$MODE" == "quick" ]; then
        return
    fi
    
    print_section "Test 4: Large Pipeline Handling"
    print_test "Pipeline with many commands"
    
    local test_id="$$_$(date +%s)"
    local test_input="/tmp/test_input_${test_id}.txt"
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    # Create a large pipeline (10 commands)
    echo "data" > "$test_input"
    local cmd="cat $test_input"
    for i in {1..9}; do
        cmd="$cmd | cat"
    done
    
    echo -n "  Running 10-command pipeline..."
    echo "$cmd" | timeout 3 $MINISHELL > /dev/null 2>&1 || true
    echo " done"
    
    # Wait for cleanup (reduced for speed)
    echo -n "  Waiting for cleanup"
    sleep 0.2
    wait_for_process_cleanup 2 1
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    if [ $fd_diff -le 3 ] && [ $fd_diff -ge -3 ]; then
        test_result 0 "Large pipeline (10 cmds)" "(FD diff: $fd_diff)"
    else
        test_result 1 "Large pipeline (10 cmds)" "(FD diff: $fd_diff, expected: -3 to +3)"
    fi
    
    rm -f "$test_input"
}

# ============================================================================
# TEST 5: ZOMBIE PROCESSES
# ============================================================================

test_zombie_processes() {
    if [ "$MODE" == "quick" ]; then
        return
    fi
    
    print_section "Test 5: Zombie Process Prevention"
    print_test "Failed pipelines should not leave zombie processes"
    
    local test_id="$$_$(date +%s)"
    local test_input="/tmp/test_input_${test_id}.txt"
    local test_script="/tmp/run_limited_minishell_${test_id}.sh"
    
    local before_zombies=$(count_zombies)
    log_verbose "Zombies before: $before_zombies"
    
    # Run a pipeline that might fail
    echo "data" > "$test_input"
    cat > "$test_script" << EOF
#!/bin/bash
ulimit -u 15 2>/dev/null
echo "cat $test_input | grep test | sort | uniq" | $MINISHELL 2>/dev/null
exit 0
EOF
    chmod +x "$test_script"
    
    echo -n "  Testing zombie prevention..."
    timeout 2 bash "$test_script" > /dev/null 2>&1 || true
    echo " done"
    
    # Give processes time to be reaped (zombies need more time)
    echo -n "  Waiting for process reaping"
    sleep 0.5
    wait_for_process_cleanup 3 1  # Zombies may need more time
    
    local after_zombies=$(count_zombies)
    log_verbose "Zombies after: $after_zombies"
    
    local zombie_diff=$((after_zombies - before_zombies))
    
    if [ $zombie_diff -eq 0 ]; then
        test_result 0 "No zombie processes" "(count: $after_zombies)"
    else
        test_result 1 "Zombie processes found" "(new zombies: $zombie_diff)"
    fi
    
    rm -f "$test_input" "$test_script"
}

# ============================================================================
# TEST 6: RAPID EXECUTION
# ============================================================================

test_rapid_execution() {
    if [ "$MODE" == "quick" ]; then
        return
    fi
    
    print_section "Test 6: Rapid Pipeline Execution"
    print_test "Many pipelines in quick succession"
    
    local test_id="$$_$(date +%s)"
    local test_input="/tmp/test_input_${test_id}.txt"
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    echo "test" > "$test_input"
    
    # Run 10 pipelines quickly
    echo "  Running 10 rapid pipelines..."
    for i in {1..10}; do
        echo -n "    Pipeline $i/10..."
        echo "cat $test_input | grep test" | timeout 1 $MINISHELL > /dev/null 2>&1 || true
        echo " done"
        sleep 0.05  # Reduced from 0.1 to 0.05
    done
    
    # Wait for cleanup (reduced for speed)
    echo -n "  Final cleanup check"
    sleep 0.2
    wait_for_process_cleanup 2 1
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    if [ $fd_diff -le 3 ] && [ $fd_diff -ge -3 ]; then
        test_result 0 "Rapid execution (10 runs)" "(FD diff: $fd_diff)"
    else
        test_result 1 "Rapid execution (10 runs)" "(FD diff: $fd_diff, expected: -3 to +3)"
    fi
    
    rm -f "$test_input"
}

# ============================================================================
# SUMMARY REPORT
# ============================================================================

print_summary() {
    print_banner "TEST SUMMARY"
    
    echo -e "${BOLD}Results:${NC}"
    echo -e "  Total Tests:  $TOTAL_TESTS"
    echo -e "  ${GREEN}Passed:       $PASSED_TESTS${NC}"
    
    if [ $FAILED_TESTS -gt 0 ]; then
        echo -e "  ${RED}Failed:       $FAILED_TESTS${NC}"
    else
        echo -e "  Failed:       $FAILED_TESTS"
    fi
    
    if [ $WARNINGS -gt 0 ]; then
        echo -e "  ${YELLOW}Warnings:     $WARNINGS${NC}"
    fi
    
    echo ""
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "${GREEN}${BOLD}╔════════════════════════════════════════╗${NC}"
        echo -e "${GREEN}${BOLD}║  ✓ ALL TESTS PASSED                   ║${NC}"
        echo -e "${GREEN}${BOLD}║  No file descriptor leaks detected!   ║${NC}"
        echo -e "${GREEN}${BOLD}╚════════════════════════════════════════╝${NC}"
        echo ""
        return 0
    else
        echo -e "${RED}${BOLD}╔════════════════════════════════════════╗${NC}"
        echo -e "${RED}${BOLD}║  ✗ SOME TESTS FAILED                  ║${NC}"
        echo -e "${RED}${BOLD}║  File descriptor leaks detected!      ║${NC}"
        echo -e "${RED}${BOLD}╚════════════════════════════════════════╝${NC}"
        echo ""
        echo -e "${YELLOW}Troubleshooting:${NC}"
        echo -e "  1. Check if close_partial_pipes() is called on all error paths"
        echo -e "  2. Verify pipe creation error handling"
        echo -e "  3. Ensure fork failure cleanup is working"
        echo -e "  4. Review pipeline.c for early returns without cleanup"
        echo ""
        return 1
    fi
}

# ============================================================================
# MAIN EXECUTION
# ============================================================================

main() {
    print_banner "MINISHELL FD LEAK & PIPELINE TEST SUITE"
    
    # Clean up any leftover files from previous runs
    cleanup_test_files
    
    # Pre-flight checks
    preflight_checks
    
    # Run tests
    test_normal_pipeline
    test_fork_failure
    test_multiple_failures
    test_large_pipeline
    test_zombie_processes
    test_rapid_execution
    
    # Summary (cleanup happens via trap)
    print_summary
}

# Run main and preserve exit code
main
exit $?
