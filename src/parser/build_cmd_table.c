#include "minishell.h"

// static void    print_cmd_table(t_cmd_table *table) //for debugging purpose
// {
//     if (!table)
//     {
//         ft_printf("Command table not found\n");
//         return ;
//     }
//     ft_printf("\nPrinting command table\n");
//     ft_printf("Number of commands: %i\n", table->cmd_count);
//     t_cmd *current_cmd = table->list_of_cmds;
//     int cmd_index = 1;
    
//     while (current_cmd)
//     {
//         printf("COMMAND #%d:\n", cmd_index);
        
        
//         // Print arguments
//         printf("cmd_av: [");
//         if (current_cmd->cmd_av)
//         {
//             for (int i = 0; current_cmd->cmd_av[i] != NULL; i++)
//             {
//                 printf("\"%s\"", current_cmd->cmd_av[i]);
//                 if (current_cmd->cmd_av[i + 1] != NULL)
//                     printf(", ");
//             }
//         }
//         printf("]\n");
//         if (current_cmd->file_name)
//         {
//             printf("Redirection no %i\n", current_cmd->redirection);
//             printf("filename saved: %s\n", current_cmd->file_name);
//         }
//         printf("------------\n");
//         current_cmd = current_cmd->next_cmd;
//         cmd_index++;
//     }
// }

/*
- Free and assign as NULL each element the double pointer points to
- Free the double pointer itself and assign it as NULL
- Return NULL
*/
void    **clean_free_double_pointers(void **trash)
{
    int i;

    i = 0;
    if (trash)
    {
        while (trash[i])
        {
            free (trash[i]);
            trash[i] = NULL;
            i++;
        }
        free (trash);
        trash = NULL;
    }
    return (NULL);
}

/* Expand command array to include a new token value:
 - Allocating new array with space for existing arguments + new argument + NULL
 - Give existing string pointers to new array
 - Use ft_strdup() to duplicate the new token value into the array
 - Add NULL terminator to the expanded array
 - Update the old command array with the new allocation
*/
void    add_argv(t_cmd *command, char *expansion)
{
    size_t  quantity;
    size_t  i;
    char    **new_cmd;

    quantity = 0;
    if (command->cmd_av)
        while (command->cmd_av[quantity])
            quantity++;
    new_cmd = ft_calloc(quantity + 2, sizeof(char *));
    if (!new_cmd)
    {
        ft_printf("Allocation for command failed\n");
        command->cmd_av = clean_free_double_pointers(command); //need to make cmv_av = NULL to check in outer function
        return ;
    }
    i = 0;
    if (command->cmd_av)
        while (command->cmd_av[i])
            new_cmd[i++] = command->cmd_av[i++];
    new_cmd[i] = ft_strdup(expansion);
    if (!new_cmd[i])
    {
        ft_printf("strdup fail while building command\n");
        free(new_cmd);
        command->cmd_av = clean_free_double_pointers(command); //need to make cmv_av = NULL to check in outer function
        return ;
    }
    new_cmd[i + 1] = NULL;
    free(command->cmd_av);
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

/* Parses a linked list of tokens into a command table structure.

Builds a command table where:
- Pipes (|) separate commands and increment the command count
- Redirections (<, >, >>, <<) set redirection type and capture filename
- Regular word tokens are added as command arguments
*/
t_cmd_table *register_to_table(t_tokens *list_of_toks)
{
    t_cmd_table *table;
    t_tokens    *current_tok;
    t_cmd       *current_cmd;

    if (list_of_toks == NULL)
        return (NULL);
    current_tok = list_of_toks;    
    table = ft_calloc(1, sizeof(t_cmd_table));
    if (!table)
        return (NULL);
    current_cmd = new_cmd_alloc();
    if (!current_cmd)
    {
        free(table);
        return (NULL);
    }
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
            if (!current_cmd->next_cmd)
            {
                ft_printf("Memory allocation failed for new command\n");
                //cleanup
                return (NULL);
            }
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
            if (!current_cmd->file_name)
            {
                ft_printf("Memory allocation failed for file name\n");
                //cleanup
                return (NULL);
            }
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
        }
        current_tok = current_tok->next; //move to the next token
    }
    //print_cmd_table(table);
    return (table);
}
