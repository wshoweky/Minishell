#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void demonstrate_fd_lifecycle() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2, pid3;
    
    printf("=== FD Lifecycle in Pipeline Demo ===\n");
    
    // Create pipes
    pipe(pipe1);  // For cat -> grep
    pipe(pipe2);  // For grep -> wc
    
    printf("Parent created pipes: pipe1[%d,%d], pipe2[%d,%d]\n", 
           pipe1[0], pipe1[1], pipe2[0], pipe2[1]);
    
    // First child: cat
    if ((pid1 = fork()) == 0) {
        printf("Child 1 (cat): Started with all FDs inherited\n");
        
        // Close unused pipe ends
        close(pipe1[0]);  // Don't need read end of pipe1
        close(pipe2[0]);  // Don't need read end of pipe2
        close(pipe2[1]);  // Don't need write end of pipe2
        
        // Redirect stdout to pipe1 write end
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[1]);  // Close original after dup2
        
        printf("Child 1 (cat): About to execve - only needed FDs remain\n");
        execve("/bin/cat", (char*[]){"cat", "/etc/hostname", NULL}, NULL);
        // When cat finishes, ALL its FDs are automatically closed
        exit(1);
    }
    
    // Second child: grep
    if ((pid2 = fork()) == 0) {
        printf("Child 2 (grep): Started with all FDs inherited\n");
        
        // Close unused pipe ends
        close(pipe1[1]);  // Don't need write end of pipe1
        close(pipe2[0]);  // Don't need read end of pipe2
        
        // Redirect stdin from pipe1, stdout to pipe2
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);  // Close original after dup2
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);  // Close original after dup2
        
        printf("Child 2 (grep): About to execve - only needed FDs remain\n");
        execve("/bin/cat", (char*[]){"cat", NULL}, NULL);  // Just pass through
        // When grep finishes, ALL its FDs are automatically closed
        exit(1);
    }
    
    // Third child: wc
    if ((pid3 = fork()) == 0) {
        printf("Child 3 (wc): Started with all FDs inherited\n");
        
        // Close unused pipe ends
        close(pipe1[0]);  // Don't need read end of pipe1
        close(pipe1[1]);  // Don't need write end of pipe1
        close(pipe2[1]);  // Don't need write end of pipe2
        
        // Redirect stdin from pipe2
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);  // Close original after dup2
        
        printf("Child 3 (wc): About to execve - only needed FDs remain\n");
        execve("/bin/wc", (char*[]){"wc", "-l", NULL}, NULL);
        // When wc finishes, ALL its FDs are automatically closed
        exit(1);
    }
    
    // Parent closes all pipe ends (crucial!)
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    
    printf("Parent: Closed all pipe FDs, waiting for children\n");
    
    // Wait for all children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
    
    printf("Parent: All children finished - their FDs automatically closed\n");
}

int main() {
    demonstrate_fd_lifecycle();
    return 0;
}