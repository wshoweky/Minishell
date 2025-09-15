#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

// cc add_history.c -lreadline

void    handle_input(char *line)
{
    if (line && *line) // only add non-empty lines
        add_history(line);

    if (line && strcmp(line, "exit") == 0)
    {
        free(line);
        exit(0);
    }
    printf("You typed: %s\n", line);
}

int main(void)
{
    char *line;

    while (1)
    {
        line = readline("add_history> "); // prompt
        if (!line) // handle Ctrl+D
        {
            printf("exit\n");
            break;
        }
        handle_input(line);
        free(line); // readline allocates memory → free it
    }
    return (0);
}

