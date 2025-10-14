#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>

void show_open_fds(const char* label, pid_t pid) {
    char proc_path[256];
    DIR *proc_dir;
    struct dirent *entry;
    int count = 0;
    
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/fd", pid);
    proc_dir = opendir(proc_path);
    
    printf("%s (PID %d):\n", label, pid);
    if (proc_dir) {
        while ((entry = readdir(proc_dir)) != NULL) {
            if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
                printf("  FD %s\n", entry->d_name);
                count++;
            }
        }
        closedir(proc_dir);
    }
    printf("  Total FDs: %d\n\n", count);
}

int main() {
    int pipe_fds[2];
    pid_t pid;
    
    printf("=== EXECVE FD INHERITANCE TEST ===\n\n");
    
    // Create a pipe
    if (pipe(pipe_fds) < 0) {
        perror("pipe");
        return 1;
    }
    
    printf("Created pipe: read_fd=%d, write_fd=%d\n", pipe_fds[0], pipe_fds[1]);
    
    // Show parent FDs before fork
    show_open_fds("Parent before fork", getpid());
    
    pid = fork();
    if (pid == 0) {
        // Child process
        printf("Child before execve:\n");
        show_open_fds("Child before execve", getpid());
        
        // Execute a simple command that shows its FDs
        // We'll use 'ls -la /proc/self/fd' to see what FDs the exec'd program inherits
        printf("Executing 'ls -la /proc/self/fd' to see inherited FDs...\n\n");
        execl("/bin/ls", "ls", "-la", "/proc/self/fd", NULL);
        
        // This should never be reached
        perror("execl failed");
        return 1;
    } else if (pid > 0) {
        // Parent process
        int status;
        
        // Close pipe ends in parent (good practice)
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        
        // Wait for child
        wait(&status);
        
        printf("\nChild completed. Parent after wait:\n");
        show_open_fds("Parent after wait", getpid());
        
    } else {
        perror("fork");
        return 1;
    }
    
    return 0;
}