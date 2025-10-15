/**
 * DEMONSTRATION: Why children can't wait for siblings
 * 
 * This shows what happens when a child tries to waitpid() its sibling
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main(void)
{
    pid_t parent_pid = getpid();
    pid_t child1_pid, child2_pid;
    int status;
    
    printf("=== Sibling Wait Demonstration ===\n");
    printf("Parent PID: %d\n\n", parent_pid);
    
    // Fork first child
    child1_pid = fork();
    
    if (child1_pid == 0) {
        // ===== CHILD 1 PROCESS =====
        printf("[Child 1] My PID: %d, Parent: %d\n", getpid(), getppid());
        
        // Sleep to ensure Child 2 is forked
        sleep(1);
        
        // Get Child 2's PID from environment (parent will set it)
        // In real scenario, we don't know sibling PIDs, but let's try anyway
        printf("[Child 1] Attempting to wait for my sibling...\n");
        
        // Try to wait for ANY child (will fail - we have no children!)
        pid_t result = waitpid(-1, &status, 0);
        
        if (result == -1) {
            printf("[Child 1] ❌ waitpid() FAILED: %s\n", strerror(errno));
            printf("[Child 1] Error code: ECHILD (No child processes)\n");
            printf("[Child 1] Reason: I have no children, only siblings!\n");
        }
        
        printf("[Child 1] Exiting normally...\n");
        exit(0);
    }
    
    // Fork second child
    child2_pid = fork();
    
    if (child2_pid == 0) {
        // ===== CHILD 2 PROCESS =====
        printf("[Child 2] My PID: %d, Parent: %d\n", getpid(), getppid());
        printf("[Child 2] Sleeping for 2 seconds...\n");
        sleep(2);
        printf("[Child 2] Waking up and exiting...\n");
        exit(42);  // Exit with specific code
    }
    
    // ===== PARENT PROCESS =====
    printf("[Parent] Child 1 PID: %d\n", child1_pid);
    printf("[Parent] Child 2 PID: %d\n", child2_pid);
    printf("[Parent] Both children forked, now waiting for them...\n\n");
    
    // Wait for first child
    pid_t pid = waitpid(child1_pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("[Parent] ✅ Child 1 (PID %d) exited with status: %d\n",
               pid, WEXITSTATUS(status));
    }
    
    // Wait for second child
    pid = waitpid(child2_pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("[Parent] ✅ Child 2 (PID %d) exited with status: %d\n",
               pid, WEXITSTATUS(status));
    }
    
    printf("\n[Parent] All children reaped successfully!\n");
    printf("[Parent] This is the ONLY process that can wait for those children.\n");
    
    return 0;
}
