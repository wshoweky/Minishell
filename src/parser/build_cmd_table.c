#include "minishell.h"


void    print_cmd_table(t_cmd_table *table) //for debugging purpose
{
    if (!table)
    {
        ft_printf("Command table not found\n");
        return ;
    }
    ft_printf("Number of commands: %i\n", table->cmd_count);
    t_cmd *current_cmd = table->list_of_cmds;
    int cmd_index = 1;
    
    while (current_cmd)
    {
        printf("COMMAND #%d:\n", cmd_index);
        printf("------------\n");
        
        // Print arguments
        printf("Arguments: [");
        if (current_cmd->cmd_av) {
            for (int i = 0; current_cmd->cmd_av[i] != NULL; i++) {
                printf("\"%s\"", current_cmd->cmd_av[i]);
                if (current_cmd->cmd_av[i + 1] != NULL) {
                    printf(", ");
                }
            }
        }
        printf("]\n");
        current_cmd = current_cmd->next_cmd;
        cmd_index++;
    }
}


/* Update the command array with tokens passed to it
*/
void    add_argv(t_cmd *command, char *expansion)
{
	size_t  i;
    char    **new_cmd;

    i = 0;
    if (command->cmd_av)
        while (command->cmd_av[i])
            i++;
    new_cmd = ft_calloc(i + 2, sizeof(char *));
    if (!new_cmd)
    {
        ft_printf("Allocation for command failed\n");
        free (command->cmd_av);
        command->cmd_av = NULL;
        return ;
    }
    i = 0;
    if (command->cmd_av)
        while (command->cmd_av[i])
            new_cmd[i] = command->cmd_av[i];
    new_cmd[i] = ft_strdup(expansion);
    if (!new_cmd[i])
    {
        ft_printf("strdup fail while building command\n");
        free (new_cmd);
        free (command->cmd_av);
        command->cmd_av = NULL;
        return ;
    }
    new_cmd[i + 1] = NULL;
    free (command->cmd_av);
    command->cmd_av = new_cmd;
}

/* Check if token is a redirectional token.
*/
int is_redirection(t_token_type check)
{
    if (check == TOKEN_REDIRECT_IN || check == TOKEN_REDIRECT_OUT
        || check == TOKEN_APPEND || check == TOKEN_HEREDOC)
        return (8);
    return (0);
}

/* Create space with ft_calloc for a t_cmd struct so all values are 0
*/
t_cmd   *new_cmd_alloc()
{
    t_cmd   *new;

    new = ft_calloc(1, sizeof(t_cmd));
    if (!new)
        return (NULL);
    return (new);
}

/* Make a command table while going through the tokens linked list:
- Count how many commands there are and erase the pipe with helper function
- Allocate memory for as many t_cmd structs;
- 
- If token value is a redirection -> save next token value as file name
- 
*/
t_cmd_table *register_to_table(t_tokens *list_of_toks)
{
    t_cmd_table *table;
    t_tokens    *current_tok;
    t_cmd       *current_cmd;

    if (list_of_toks == NULL)
        return (NULL);
    current_tok = list_of_toks;    
    table = ft_calloc(1, sizeof(table));
    current_cmd = new_cmd_alloc();
    table->list_of_cmds = current_cmd;
    table->cmd_count = 1;
    
    while (current_tok)
    {
        if (current_tok->type == TOKEN_PIPE)
        {
            if (!current_tok->next || current_tok->next->type != TOKEN_WORD)
            {
                ft_printf("No command after pipe\n");
                //cleanup list_of_toks, free table/token/command?
                return (NULL);
            }
            current_cmd->next_cmd = new_cmd_alloc();
            current_cmd = current_cmd->next_cmd; //prev cmd done, onto new cmd
            table->cmd_count++;
        }
        else if (is_redirection(current_tok->type))
        {
            if (current_tok->type == TOKEN_REDIRECT_IN)
                current_cmd->redirection = TOKEN_REDIRECT_IN;
            else if (current_tok->type == TOKEN_REDIRECT_OUT)
                current_cmd->redirection = TOKEN_REDIRECT_OUT;
            else if (current_tok->type == TOKEN_APPEND)
                current_cmd->redirection = TOKEN_APPEND;
            else if (current_tok->type == TOKEN_HEREDOC)
                current_cmd->redirection = TOKEN_HEREDOC;
            if (!current_tok->next || current_tok->next->type != TOKEN_WORD)
            {
                ft_printf("No valid name for redirection file\n");
                //cleanup;
                return (NULL);
            }
            current_tok = current_tok->next;
            current_cmd->file_name = ft_strdup(current_tok->value);
        }
        else
        {
            if (current_tok->type != TOKEN_WORD)
            {
                ft_printf("Not a word token\n");
                //cleanup;
                return (NULL);
            }
            add_argv(current_cmd, current_tok->value);
            if (!current_cmd->cmd_av)
            {
                ft_printf("Adding command argv unsuccessful\n");
                //cleanup;
                return (NULL);
            }
            ft_printf("got here after add_argv\n");
        }
        current_tok = current_tok->next; //move to the next token
    }
    ft_printf("got here before print table\n");
    print_cmd_table(table);
    return (table);
}

