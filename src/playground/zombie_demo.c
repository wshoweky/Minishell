/**
 * DEMONSTRATION: What are zombie processes?
 * 
 * Shows what happens when a parent DOESN'T wait for its children
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void show_process_status(pid_t pid, const char *name)
{
    char cmd[256];
    printf("\n--- Process status for %s (PID %d) ---\n", name, pid);
    
    // Use ps to show process state
    sprintf(cmd, "ps -o pid,ppid,stat,cmd -p %d 2>/dev/null || echo 'Process %d not found'", pid, pid);
    system(cmd);
    
    // Show /proc info if available
    sprintf(cmd, "cat /proc/%d/status 2>/dev/null | grep -E '^(Name|State|PPid):' || true", pid);
    system(cmd);
}

int main(void)
{
    pid_t child_pid;
    
    printf("=== Zombie Process Demonstration ===\n");
    printf("Parent PID: %d\n", getpid());
    
    child_pid = fork();
    
    if (child_pid == 0) {
        // ===== CHILD PROCESS =====
        printf("\n[Child] My PID: %d\n", getpid());
        printf("[Child] I'm going to exit now...\n");
        printf("[Child] After I exit, I become a ZOMBIE until parent waits for me!\n");
        exit(42);  // Child exits here
    }
    
    // ===== PARENT PROCESS =====
    printf("\n[Parent] Child PID: %d\n", child_pid);
    printf("[Parent] Child has been forked and will exit shortly...\n");
    
    // Give child time to exit
    sleep(1);
    
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  ZOMBIE STATE: Child has exited but parent hasn't waited ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    
    show_process_status(child_pid, "Child (ZOMBIE)");
    
    printf("\n[Parent] Look for 'Z' or 'Z+' in STAT column - that's ZOMBIE!\n");
    printf("[Parent] Or 'State: Z (zombie)' in /proc status\n");
    printf("[Parent] The child is DEAD but its entry remains in process table\n");
    
    printf("\n--- What is stored for the zombie? ---\n");
    printf("1. Exit status (42 in this case)\n");
    printf("2. Resource usage stats (CPU time, memory used, etc.)\n");
    printf("3. Process ID\n");
    printf("4. Termination reason (normal exit vs signal)\n");
    
    printf("\n[Parent] Sleeping for 5 seconds with zombie child...\n");
    printf("[Parent] During this time, you can run: ps aux | grep Z\n");
    printf("[Parent] Or: ps -o pid,ppid,stat,cmd -p %d\n", child_pid);
    sleep(5);
    
    printf("\n[Parent] Now calling waitpid() to REAP the zombie...\n");
    int status;
    pid_t result = waitpid(child_pid, &status, 0);
    
    if (result > 0 && WIFEXITED(status)) {
        printf("[Parent] ✅ Zombie REAPED! Retrieved exit status: %d\n", 
               WEXITSTATUS(status));
    }
    
    printf("\n[Parent] After wait(), the child process entry is removed:\n");
    show_process_status(child_pid, "Child (REAPED)");
    
    printf("\n=== Why wait() is necessary ===\n");
    printf("1. Retrieve exit status (we got 42)\n");
    printf("2. Free process table entry (prevent zombie accumulation)\n");
    printf("3. Collect resource usage statistics\n");
    printf("4. Prevent system resource leaks\n");
    
    printf("\n=== What happens with many zombies? ===\n");
    printf("• Each zombie takes a process table slot\n");
    printf("• System has LIMITED process table (ulimit -u)\n");
    printf("• Too many zombies = can't fork new processes!\n");
    printf("• Error: 'fork: Resource temporarily unavailable'\n");
    
    return 0;
}
