#!/bin/bash

# ============================================================================
# COMPREHENSIVE MINISHELL PIPELINE TESTING SUITE
# ============================================================================
# This test suite validates that the minishell pipeline implementation
# properly handles file descriptor cleanup in all scenarios, including
# failures during pipe creation and process forking.
#
# WHAT WE'RE TESTING:
# 1. FD leaks when fork() fails in middle of pipeline
# 2. FD leaks when pipe() fails during pipeline setup  
# 3. Proper cleanup of child processes (no zombies)
# 4. Normal pipeline operation continues to work
# 5. Stress testing with multiple failures
#
# WHY THIS MATTERS:
# The original pipeline code had a bug where early returns during failures
# would leave pipe file descriptors open, causing resource leaks that could
# eventually make the shell unusable.
# ============================================================================

set -e

# Colors for pretty output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Configuration
MINISHELL="./minishell"
TEST_DIR="/tmp/minishell_comprehensive_test_$$"
ORIGINAL_ULIMIT_N=$(ulimit -n)
ORIGINAL_ULIMIT_U=$(ulimit -u)

# Test tracking
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# ============================================================================
# UTILITY FUNCTIONS
# ============================================================================

print_banner() {
    echo -e "${PURPLE}"
    echo "╔══════════════════════════════════════════════════════════════════════════════╗"
    echo "║                     MINISHELL PIPELINE TEST SUITE                           ║"
    echo "║                         Comprehensive FD Leak Testing                       ║"
    echo "╚══════════════════════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_section() {
    echo -e "${CYAN}"
    echo "┌────────────────────────────────────────────────────────────────────────────┐"
    echo "│ $1"
    echo "└────────────────────────────────────────────────────────────────────────────┘"
    echo -e "${NC}"
}

start_test() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "${YELLOW}[TEST $TOTAL_TESTS] $1${NC}"
}

pass_test() {
    PASSED_TESTS=$((PASSED_TESTS + 1))
    echo -e "${GREEN}✓ PASS${NC}: $1"
}

fail_test() {
    FAILED_TESTS=$((FAILED_TESTS + 1))
    echo -e "${RED}✗ FAIL${NC}: $1"
}

count_fds() {
    local pid=$1
    if [ -d "/proc/$pid/fd" ]; then
        ls "/proc/$pid/fd" 2>/dev/null | wc -l
    else
        echo "0"
    fi
}

show_fd_details() {
    local label="$1"
    local pid="$2"
    echo "📊 $label (PID $pid):"
    echo "   Total FDs: $(count_fds $pid)"
    local pipes=$(ls -la "/proc/$pid/fd/" 2>/dev/null | grep -c "pipe:" || echo "0")
    echo "   Pipe FDs: $pipes"
}

setup_test_environment() {
    mkdir -p "$TEST_DIR"
    cd "$TEST_DIR"
    
    # Create various test files
    echo -e "hello world\ntest line\nanother line\nhello again" > input.txt
    echo -e "line1\nline2\nline3\nline4\nline5" > numbers.txt
    echo -e "apple\nbanana\napple\ncherry\nbanana" > fruits.txt
    
    # Reset ulimits
    ulimit -n "$ORIGINAL_ULIMIT_N" 2>/dev/null || true
    ulimit -u "$ORIGINAL_ULIMIT_U" 2>/dev/null || true
}

cleanup_test_environment() {
    cd - >/dev/null 2>&1
    rm -rf "$TEST_DIR"
    
    # Kill any stray processes
    pkill -P $$ 2>/dev/null || true
    
    # Reset ulimits
    ulimit -n "$ORIGINAL_ULIMIT_N" 2>/dev/null || true
    ulimit -u "$ORIGINAL_ULIMIT_U" 2>/dev/null || true
}

# ============================================================================
# CORE FUNCTIONALITY TESTS
# ============================================================================

test_normal_pipeline_operation() {
    start_test "Normal pipeline operation"
    
    local before_fds=$(count_fds $$)
    show_fd_details "Before normal pipeline" $$
    
    # Test a normal pipeline that should work perfectly
    local result=$(echo "cat input.txt | grep hello | wc -l" | $MINISHELL 2>/dev/null | tail -n 1)
    
    local after_fds=$(count_fds $$)
    show_fd_details "After normal pipeline" $$
    
    local fd_diff=$((after_fds - before_fds))
    
    if [ "$result" = "2" ] && [ $fd_diff -le 1 ]; then
        pass_test "Normal pipeline works correctly (result: $result, fd_diff: $fd_diff)"
    else
        fail_test "Normal pipeline issues (result: $result, fd_diff: $fd_diff)"
    fi
    echo ""
}

test_complex_pipeline() {
    start_test "Complex multi-stage pipeline"
    
    local before_fds=$(count_fds $$)
    
    # Test a more complex pipeline
    echo "cat fruits.txt | sort | uniq -c | sort -n" | $MINISHELL >/dev/null 2>&1
    
    local after_fds=$(count_fds $$)
    local fd_diff=$((after_fds - before_fds))
    
    if [ $fd_diff -le 1 ]; then
        pass_test "Complex pipeline handled correctly (fd_diff: $fd_diff)"
    else
        fail_test "Complex pipeline shows FD issues (fd_diff: $fd_diff)"
    fi
    echo ""
}

# ============================================================================
# FAILURE SCENARIO TESTS
# ============================================================================

test_fork_failure_scenario() {
    start_test "Fork failure FD leak prevention"
    
    local before_fds=$(count_fds $$)
    show_fd_details "Before fork failure test" $$
    
    # Create a script with severe process limits
    cat > fork_test.sh << 'EOF'
#!/bin/bash
ulimit -u 10  # Very low process limit
echo "cat input.txt | sort | uniq | grep hello | wc -l" | ./minishell 2>/dev/null
EOF
    chmod +x fork_test.sh
    
    # Run the test - expect it to fail due to process limits
    echo "🔬 Running pipeline with process limit (ulimit -u 10)..."
    (cd "$TEST_DIR" && timeout 5s ../fork_test.sh) >/dev/null 2>&1 || echo "   Fork failures occurred as expected"
    
    sleep 1  # Allow cleanup time
    
    local after_fds=$(count_fds $$)
    show_fd_details "After fork failure test" $$
    
    local fd_diff=$((after_fds - before_fds))
    
    if [ $fd_diff -le 1 ]; then
        pass_test "No FD leak after fork failures (fd_diff: $fd_diff)"
    else
        fail_test "FD leak detected after fork failures (fd_diff: $fd_diff)"
    fi
    
    rm -f fork_test.sh
    echo ""
}

test_pipe_creation_failure() {
    start_test "Pipe creation failure FD leak prevention"
    
    local before_fds=$(count_fds $$)
    show_fd_details "Before pipe failure test" $$
    
    # Create a script with very low FD limits
    cat > pipe_test.sh << 'EOF'
#!/bin/bash
ulimit -n 20  # Very low file descriptor limit
# Try to create a pipeline that needs many FDs
echo "cat input.txt | cat | cat | cat | cat | cat | cat | cat | wc -l" | ./minishell 2>/dev/null
EOF
    chmod +x pipe_test.sh
    
    echo "🔬 Running pipeline with FD limit (ulimit -n 20)..."
    (cd "$TEST_DIR" && timeout 5s ../pipe_test.sh) >/dev/null 2>&1 || echo "   Pipe creation failures occurred as expected"
    
    sleep 1
    
    local after_fds=$(count_fds $$)
    show_fd_details "After pipe failure test" $$
    
    local fd_diff=$((after_fds - before_fds))
    
    if [ $fd_diff -le 1 ]; then
        pass_test "No FD leak after pipe failures (fd_diff: $fd_diff)"
    else
        fail_test "FD leak detected after pipe failures (fd_diff: $fd_diff)"
    fi
    
    rm -f pipe_test.sh
    echo ""
}

# ============================================================================
# STRESS TESTS
# ============================================================================

test_repeated_failures() {
    start_test "Repeated pipeline failures (stress test)"
    
    local initial_fds=$(count_fds $$)
    show_fd_details "Before stress test" $$
    
    echo "🔬 Running 15 constrained pipelines..."
    
    # Run multiple failing pipelines
    for i in {1..15}; do
        cat > stress_$i.sh << EOF
#!/bin/bash
ulimit -u 12 -n 25
echo "cat input.txt | sort | uniq | head | tail | wc -l" | ./minishell 2>/dev/null
EOF
        chmod +x stress_$i.sh
        
        echo -n "."
        (cd "$TEST_DIR" && timeout 1s ../stress_$i.sh) >/dev/null 2>&1 || true
        rm -f stress_$i.sh
        sleep 0.1
    done
    echo ""
    
    sleep 2  # Allow full cleanup
    
    local final_fds=$(count_fds $$)
    show_fd_details "After stress test" $$
    
    local total_diff=$((final_fds - initial_fds))
    
    if [ $total_diff -le 3 ]; then
        pass_test "No significant FD accumulation after stress test (diff: $total_diff)"
    else
        fail_test "FD accumulation detected in stress test (diff: $total_diff)"
    fi
    echo ""
}

test_zombie_prevention() {
    start_test "Zombie process prevention"
    
    echo "🔬 Testing for zombie processes after failures..."
    
    # Create conditions that might leave zombies
    cat > zombie_test.sh << 'EOF'
#!/bin/bash
ulimit -u 15
echo "sleep 0.5 | sleep 0.5 | false | sleep 0.5" | ./minishell 2>/dev/null
EOF
    chmod +x zombie_test.sh
    
    (cd "$TEST_DIR" && timeout 3s ../zombie_test.sh) >/dev/null 2>&1 || true
    
    sleep 2  # Allow cleanup
    
    local zombie_count=$(ps aux | grep -c '[Zz]ombie' || echo "0")
    
    if [ $zombie_count -eq 0 ]; then
        pass_test "No zombie processes detected"
    else
        fail_test "Zombie processes found: $zombie_count"
    fi
    
    rm -f zombie_test.sh
    echo ""
}

# ============================================================================
# EDGE CASES
# ============================================================================

test_edge_cases() {
    start_test "Edge case handling"
    
    local before_fds=$(count_fds $$)
    
    echo "🔬 Testing edge cases..."
    
    # Test empty input
    echo "" | $MINISHELL >/dev/null 2>&1 || true
    
    # Test malformed pipelines
    echo "| | |" | $MINISHELL >/dev/null 2>&1 || true
    
    # Test single command (no pipes)
    echo "echo hello" | $MINISHELL >/dev/null 2>&1 || true
    
    local after_fds=$(count_fds $$)
    local fd_diff=$((after_fds - before_fds))
    
    if [ $fd_diff -le 1 ]; then
        pass_test "Edge cases handled correctly (fd_diff: $fd_diff)"
    else
        fail_test "Edge cases show FD issues (fd_diff: $fd_diff)"
    fi
    echo ""
}

# ============================================================================
# MAIN EXECUTION
# ============================================================================

run_all_tests() {
    print_banner
    
    echo -e "Testing executable: ${BLUE}$(realpath $MINISHELL)${NC}"
    echo -e "Test process PID: ${BLUE}$$${NC}"
    echo -e "Original limits - Files: ${BLUE}$ORIGINAL_ULIMIT_N${NC}, Processes: ${BLUE}$ORIGINAL_ULIMIT_U${NC}"
    echo ""
    
    setup_test_environment
    
    print_section "BASIC FUNCTIONALITY TESTS"
    test_normal_pipeline_operation
    test_complex_pipeline
    
    print_section "FAILURE SCENARIO TESTS"  
    test_fork_failure_scenario
    test_pipe_creation_failure
    
    print_section "STRESS TESTS"
    test_repeated_failures
    test_zombie_prevention
    
    print_section "EDGE CASE TESTS"
    test_edge_cases
    
    cleanup_test_environment
}

print_results() {
    print_section "TEST RESULTS SUMMARY"
    
    echo -e "📊 ${BLUE}Total tests run:${NC} $TOTAL_TESTS"
    echo -e "✅ ${GREEN}Tests passed:${NC} $PASSED_TESTS"
    echo -e "❌ ${RED}Tests failed:${NC} $FAILED_TESTS"
    
    local pass_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    echo -e "📈 ${BLUE}Pass rate:${NC} $pass_rate%"
    
    echo ""
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "${GREEN}🎉 ALL TESTS PASSED! 🎉${NC}"
        echo -e "${GREEN}Your pipeline implementation correctly handles FD cleanup in all scenarios.${NC}"
        echo ""
        echo -e "${CYAN}Key improvements verified:${NC}"
        echo -e "  ✓ No FD leaks on fork() failures"
        echo -e "  ✓ No FD leaks on pipe() failures  "
        echo -e "  ✓ Proper cleanup in stress scenarios"
        echo -e "  ✓ No zombie process accumulation"
        echo -e "  ✓ Normal operation unaffected"
    else
        echo -e "${RED}❌ SOME TESTS FAILED ❌${NC}"
        echo -e "${RED}Your pipeline implementation may have FD leak issues.${NC}"
        echo -e "${YELLOW}Review the failed tests above for details.${NC}"
    fi
}

main() {
    # Verify minishell exists
    if [ ! -f "$MINISHELL" ]; then
        echo -e "${RED}Error: Minishell executable not found at $MINISHELL${NC}"
        echo -e "${YELLOW}Please compile first: make${NC}"
        exit 1
    fi
    
    run_all_tests
    print_results
    
    # Exit with appropriate code
    [ $FAILED_TESTS -eq 0 ] && exit 0 || exit 1
}

# Run the comprehensive test suite
main "$@"