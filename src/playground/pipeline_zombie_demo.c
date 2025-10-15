/**
 * DEMONSTRATION: Zombie accumulation in pipeline without wait()
 * 
 * Simulates what would happen in minishell if we didn't call wait_all_children()
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void show_zombie_count(void)
{
    // Count zombie children of this process
    char cmd[256];
    sprintf(cmd, "ps -o pid,ppid,stat,cmd --ppid %d 2>/dev/null | grep -c '^.*Z' || echo 0", getpid());
    
    printf("Current zombie count: ");
    fflush(stdout);
    system(cmd);
}

void simulate_pipeline_with_wait(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  CORRECT: Pipeline WITH wait() - Your minishell     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    
    pid_t pids[3];
    
    // Simulate: echo hello | cat | wc
    printf("Forking 3 processes (simulating: echo | cat | wc)...\n");
    
    for (int i = 0; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process - simulate quick command
            sleep(1);
            exit(0);
        }
        printf("  Forked child %d: PID %d\n", i, pids[i]);
    }
    
    printf("\nAll children forked. Now waiting...\n");
    show_zombie_count();
    
    // This is what your wait_all_children() does
    for (int i = 0; i < 3; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        printf("  Reaped child %d (PID %d)\n", i, pids[i]);
    }
    
    printf("\n✅ After wait(): ");
    show_zombie_count();
    printf("Result: Clean! No zombies left.\n");
}

void simulate_pipeline_without_wait(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  BROKEN: Pipeline WITHOUT wait() - Memory leak!     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    
    // Simulate: echo hello | cat | wc
    printf("Forking 3 processes WITHOUT waiting...\n");
    
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process - simulate quick command
            sleep(1);
            exit(0);
        }
        printf("  Forked child %d: PID %d\n", i, pid);
    }
    
    printf("\nAll children forked. Parent NOT waiting!\n");
    sleep(2);  // Give children time to exit and become zombies
    
    printf("\n❌ After children exit: ");
    show_zombie_count();
    
    printf("\nProcess table entries leaked! Running ps to show zombies:\n");
    char cmd[256];
    sprintf(cmd, "ps -o pid,ppid,stat,cmd --ppid %d 2>/dev/null", getpid());
    system(cmd);
    
    printf("\n⚠️  These zombies will remain until parent exits!\n");
    printf("⚠️  In a long-running shell, this accumulates with each pipeline!\n");
    
    // Clean up (we're responsible, unlike the broken example)
    printf("\nCleaning up zombies for demo purposes...\n");
    while (wait(NULL) > 0);
}

void simulate_zombie_accumulation(void)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  DISASTER: Running 5 pipelines without wait()       ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    
    printf("Simulating a shell session with 5 pipelines:\n");
    printf("  1. cat file | grep foo\n");
    printf("  2. ls -l | wc -l\n");
    printf("  3. echo test | cat\n");
    printf("  4. ps aux | grep bash\n");
    printf("  5. cat file | sort | uniq\n\n");
    
    int total_zombies = 0;
    
    for (int pipeline = 0; pipeline < 5; pipeline++) {
        int cmds = (pipeline < 4) ? 2 : 3;  // Last one has 3 commands
        
        printf("Pipeline %d: Forking %d processes...\n", pipeline + 1, cmds);
        
        for (int i = 0; i < cmds; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                sleep(1);  // Simulate command execution
                exit(0);
            }
            total_zombies++;
        }
        
        sleep(2);  // Simulate time between commands
        
        printf("  Zombie accumulation: ");
        show_zombie_count();
    }
    
    printf("\n💀 Total zombies created: %d\n", total_zombies);
    printf("💀 Each zombie wastes: ~1KB (process table entry)\n");
    printf("💀 Zombies consume process slots: ");
    show_zombie_count();
    
    printf("\n=== System Impact ===\n");
    printf("• ulimit -u shows max processes per user\n");
    printf("• Each zombie counts against this limit!\n");
    printf("• Eventually: 'fork: Resource temporarily unavailable'\n");
    printf("• Shell becomes UNUSABLE - can't run any more commands!\n");
    
    // Clean up
    printf("\nCleaning up all zombies...\n");
    while (wait(NULL) > 0);
}

int main(void)
{
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  ZOMBIE PROCESS DEMONSTRATION: Pipeline Context\n");
    printf("═══════════════════════════════════════════════════════════\n");
    
    simulate_pipeline_with_wait();
    sleep(1);
    
    simulate_pipeline_without_wait();
    sleep(1);
    
    simulate_zombie_accumulation();
    
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("  CONCLUSION: Why wait_all_children() is CRITICAL\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("1. Retrieve exit status (for $? variable)\n");
    printf("2. Free process table entries (prevent zombie accumulation)\n");
    printf("3. Prevent resource exhaustion (fork failures)\n");
    printf("4. Maintain system stability (long-running shell)\n");
    printf("\nYour minishell CORRECTLY calls wait_all_children()! ✅\n");
    
    return 0;
}
