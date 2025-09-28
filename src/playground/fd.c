#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

/*
** strace -e trace=open,openat,close,dup2,pipe ./your_program
** valgrind --leak-check=full --track-fds=yes ./your_program
** lsof -c your_program
**
** print_fds - Debug utility to display all open file descriptors
**
** DESCRIPTION:
**   Opens /proc/self/fd directory and reads symbolic links to show
**   what each file descriptor is connected to (files, sockets, etc.)
**
** PARAMETERS:
**   msg - String to identify when this function was called
**
** HOW IT WORKS:
**   1. Opens /proc/self/fd (virtual filesystem showing all FDs)
**   2. Reads each directory entry (represents one FD)
**   3. Uses readlink() to see where each FD points to
**   4. Prints fd_number -> destination for each open FD
**
** USAGE:
**   print_fds("Before opening file");     // Shows initial FDs
**   print_fds("After opening file");      // Shows new FD was created
**   print_fds("After dup2()");           // Shows how dup2() changes FDs
*/
void print_fds(const char *msg)
{
    DIR *dir;
    struct dirent *entry;
    char path[256], link[256];
    ssize_t len;

    // Print header with the message to identify when this was called
    printf("\n--- %s ---\n", msg);
    
    // Open the /proc/self/fd directory - this shows all open file descriptors
    // /proc/self refers to the current process, /fd shows all FDs
    dir = opendir("/proc/self/fd");
    if (!dir)
    {
        perror("opendir");
        exit(1);
    }
    
    // Read each entry in the directory (each entry represents one FD)
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip hidden files like "." and ".."
        if (entry->d_name[0] == '.')
            continue;

        // Build the full path to this FD's symbolic link
        // e.g., "/proc/self/fd/0", "/proc/self/fd/1", etc.
        snprintf(path, sizeof(path), "/proc/self/fd/%s", entry->d_name);
        
        // Read the symbolic link to see where this FD points to
        // e.g., fd 1 might point to "socket:[12345]" or "/path/to/file"
        len = readlink(path, link, sizeof(link) - 1);
        if (len != -1)
        {
            // Null-terminate the link string
            link[len] = '\0';
            // Print: fd_number -> destination
            printf("fd %s -> %s\n", entry->d_name, link);
        }
    }
    
    // Close the directory when done
    closedir(dir);
}

int main(void)
{
    int fd;

    print_fds("Before opening file");

    fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }
    print_fds("After opening output.txt");

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        return 1;
    }
    print_fds("After dup2(fd, STDOUT_FILENO)");

    printf("This will now go into output.txt\n");

    close(fd);
    return 0;
}
