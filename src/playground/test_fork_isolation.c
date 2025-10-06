#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    // Set initial environment variable
    setenv("TEST_VAR", "parent_value", 1);
    printf("Before fork - TEST_VAR: %s\n", getenv("TEST_VAR"));
    
    // Change to a known directory
    chdir("/tmp");
    printf("Before fork - PWD: %s\n", getcwd(NULL, 0));
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        printf("\n=== CHILD PROCESS ===\n");
        setenv("TEST_VAR", "child_changed", 1);
        chdir("/");
        printf("Child changed TEST_VAR to: %s\n", getenv("TEST_VAR"));
        printf("Child changed PWD to: %s\n", getcwd(NULL, 0));
        exit(0);
    } else {
        // Parent process
        wait(NULL);  // Wait for child to finish
        printf("\n=== PARENT PROCESS (after child) ===\n");
        printf("Parent TEST_VAR: %s\n", getenv("TEST_VAR"));
        printf("Parent PWD: %s\n", getcwd(NULL, 0));
    }
    
    return 0;
}