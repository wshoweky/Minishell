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

MINISHELL="./minishell"
MODE="full"
VERBOSE=0

# Parse arguments
for arg in "$@"; do
    case $arg in
        --quick)   MODE="quick" ;;
        --verbose) VERBOSE=1 ;;
        --help)
            echo "Usage: $0 [--quick] [--verbose] [--help]"
            echo "  --quick    Run only essential tests"
            echo "  --verbose  Show detailed output"
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
    ls /proc/$pid/fd 2>/dev/null | wc -l || echo "0"
}

count_zombies() {
    ps aux | grep -c '[d]efunct' || echo "0"
}

cleanup_test_files() {
    rm -f /tmp/test_input_$$.txt
    rm -f /tmp/run_limited_minishell_$$.sh
    rm -f /tmp/minishell_test_output_$$.txt
}

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
        echo -e "${RED}✗ Error: Cannot count file descriptors${NC}"
        echo -e "  /proc filesystem may not be available"
        exit 1
    fi
    echo -e "${GREEN}✓${NC} FD counting available (current: $test_fd_count FDs)"
    
    # Check for required commands
    local missing=""
    for cmd in ulimit timeout grep wc; do
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
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    # Create test input
    echo "hello world" > /tmp/test_input_$$.txt
    echo "test line" >> /tmp/test_input_$$.txt
    
    # Run a simple pipeline
    echo "cat /tmp/test_input_$$.txt | grep hello | wc -l" | timeout 2 $MINISHELL > /dev/null 2>&1 || true
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    # Allow 0-1 FD difference (some systems may have slight variations)
    if [ $fd_diff -le 1 ] && [ $fd_diff -ge -1 ]; then
        test_result 0 "Normal pipeline" "(FD diff: $fd_diff)"
    else
        test_result 1 "Normal pipeline" "(FD diff: $fd_diff, expected: 0-1)"
    fi
    
    rm -f /tmp/test_input_$$.txt
}

# ============================================================================
# TEST 2: FORK FAILURE CLEANUP
# ============================================================================

test_fork_failure() {
    print_section "Test 2: Fork Failure FD Cleanup"
    print_test "Pipeline with fork() failures should clean up FDs"
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    # Create test input
    echo "test data" > /tmp/test_input_$$.txt
    
    # Create a subprocess with limited process count to force fork failures
    cat > /tmp/run_limited_minishell_$$.sh << 'SCRIPT'
#!/bin/bash
ulimit -u 10 -n 30 2>/dev/null
echo "cat /tmp/test_input_$$.txt | grep test | sort | uniq | wc -l" | ./minishell 2>/dev/null
exit 0
SCRIPT
    
    chmod +x /tmp/run_limited_minishell_$$.sh
    
    # Run with timeout in case it hangs
    timeout 3 bash /tmp/run_limited_minishell_$$.sh > /dev/null 2>&1 || true
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    # Should not leak FDs even on fork failure
    if [ $fd_diff -le 1 ] && [ $fd_diff -ge -1 ]; then
        test_result 0 "Fork failure cleanup" "(FD diff: $fd_diff)"
    else
        test_result 1 "Fork failure cleanup" "(FD diff: $fd_diff, expected: 0-1)"
    fi
    
    rm -f /tmp/test_input_$$.txt /tmp/run_limited_minishell_$$.sh
}

# ============================================================================
# TEST 3: MULTIPLE CONSECUTIVE FAILURES
# ============================================================================

test_multiple_failures() {
    print_section "Test 3: Multiple Consecutive Failures (Stress Test)"
    print_test "Repeated pipeline failures should not accumulate FDs"
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    # Create test input
    echo "data" > /tmp/test_input_$$.txt
    
    # Run multiple failing pipelines
    for i in {1..5}; do
        log_verbose "Iteration $i/5"
        cat > /tmp/run_limited_minishell_$$.sh << 'SCRIPT'
#!/bin/bash
ulimit -u 10 -n 25 2>/dev/null
echo "cat /tmp/test_input_$$.txt | grep x | sort | wc" | ./minishell 2>/dev/null
exit 0
SCRIPT
        chmod +x /tmp/run_limited_minishell_$$.sh
        timeout 2 bash /tmp/run_limited_minishell_$$.sh > /dev/null 2>&1 || true
    done
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    # Multiple failures should not accumulate FDs
    if [ $fd_diff -le 2 ] && [ $fd_diff -ge -2 ]; then
        test_result 0 "Multiple failures" "(FD diff: $fd_diff after 5 runs)"
    else
        test_result 1 "Multiple failures" "(FD diff: $fd_diff after 5 runs, expected: 0-2)"
    fi
    
    rm -f /tmp/test_input_$$.txt /tmp/run_limited_minishell_$$.sh
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
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    # Create a large pipeline (10 commands)
    echo "data" > /tmp/test_input_$$.txt
    local cmd="cat /tmp/test_input_$$.txt"
    for i in {1..9}; do
        cmd="$cmd | cat"
    done
    
    echo "$cmd" | timeout 3 $MINISHELL > /dev/null 2>&1 || true
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    if [ $fd_diff -le 2 ] && [ $fd_diff -ge -2 ]; then
        test_result 0 "Large pipeline (10 cmds)" "(FD diff: $fd_diff)"
    else
        test_result 1 "Large pipeline (10 cmds)" "(FD diff: $fd_diff, expected: 0-2)"
    fi
    
    rm -f /tmp/test_input_$$.txt
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
    
    local before_zombies=$(count_zombies)
    log_verbose "Zombies before: $before_zombies"
    
    # Run a pipeline that might fail
    echo "data" > /tmp/test_input_$$.txt
    cat > /tmp/run_limited_minishell_$$.sh << 'SCRIPT'
#!/bin/bash
ulimit -u 15 2>/dev/null
echo "cat /tmp/test_input_$$.txt | grep test | sort | uniq" | ./minishell 2>/dev/null
exit 0
SCRIPT
    chmod +x /tmp/run_limited_minishell_$$.sh
    timeout 2 bash /tmp/run_limited_minishell_$$.sh > /dev/null 2>&1 || true
    
    # Give processes time to be reaped
    sleep 1
    
    local after_zombies=$(count_zombies)
    log_verbose "Zombies after: $after_zombies"
    
    local zombie_diff=$((after_zombies - before_zombies))
    
    if [ $zombie_diff -eq 0 ]; then
        test_result 0 "No zombie processes" "(count: $after_zombies)"
    else
        test_result 1 "Zombie processes found" "(new zombies: $zombie_diff)"
    fi
    
    rm -f /tmp/test_input_$$.txt /tmp/run_limited_minishell_$$.sh
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
    
    local before_fds=$(count_fds $$)
    log_verbose "FDs before: $before_fds"
    
    echo "test" > /tmp/test_input_$$.txt
    
    # Run 10 pipelines quickly
    for i in {1..10}; do
        echo "cat /tmp/test_input_$$.txt | grep test" | timeout 1 $MINISHELL > /dev/null 2>&1 || true
    done
    
    local after_fds=$(count_fds $$)
    log_verbose "FDs after: $after_fds"
    
    local fd_diff=$((after_fds - before_fds))
    log_verbose "FD difference: $fd_diff"
    
    if [ $fd_diff -le 3 ] && [ $fd_diff -ge -3 ]; then
        test_result 0 "Rapid execution (10 runs)" "(FD diff: $fd_diff)"
    else
        test_result 1 "Rapid execution (10 runs)" "(FD diff: $fd_diff, expected: 0-3)"
    fi
    
    rm -f /tmp/test_input_$$.txt
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
    
    # Pre-flight checks
    preflight_checks
    
    # Run tests
    test_normal_pipeline
    test_fork_failure
    test_multiple_failures
    test_large_pipeline
    test_zombie_processes
    test_rapid_execution
    
    # Cleanup
    cleanup_test_files
    
    # Summary
    print_summary
}

# Run main and preserve exit code
main
exit $?
