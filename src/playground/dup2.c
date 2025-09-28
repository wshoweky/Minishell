#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

// $ ./a.out &
// $ lsof -p <pid>
// or
// ls -l /proc/<pid>/fd
int main(void)
{
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    // Duplicate fd into STDOUT (fd 1)
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        return 1;
    }

    printf("This will go to output.txt, not the terminal!\n");

    close(fd);
    sleep(100);
    return 0;
}
