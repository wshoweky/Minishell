#include "minishell.h"

static void    print_cmd_table(t_cmd_table *table) //for debugging purpose
{
    if (!table)
    {
        ft_printf("Command table not found\n");
        return ;
    }
    ft_printf("\nPrinting command table\n");
    ft_printf("Number of commands: %i\n", table->cmd_count);
    t_cmd *current_cmd = table->list_of_cmds;
    int cmd_index = 1;
    
    while (current_cmd)
    {
        printf("------------\n");
        printf("COMMAND #%d:\n", cmd_index);
        
        // Print arguments
        printf("cmd_av: [");
        if (current_cmd->cmd_av)
        {
            for (int i = 0; current_cmd->cmd_av[i] != NULL; i++)
            {
                printf("\"%s\"", current_cmd->cmd_av[i]);
                if (current_cmd->cmd_av[i + 1] != NULL)
                    printf(", ");
            }
        }
        printf("]\n");
        if (current_cmd->redirections)
        {
            printf("Redirection: %s\n", get_token_type_name(current_cmd->redirections->tok_type));
            printf("filename saved: %s\n", current_cmd->redirections->filename);
            while (current_cmd->redirections->next)
            {
                current_cmd->redirections = current_cmd->redirections->next;
                printf("Redirection: %s\n", get_token_type_name(current_cmd->redirections->tok_type));
                printf("filename saved: %s\n", current_cmd->redirections->filename);
            }
        }
        current_cmd = current_cmd->next_cmd;
        cmd_index++;
    }
}

/* Parses a linked list of tokens into a command table structure.

Builds a command table where:
- Pipes (|) separate commands and increment the command count
- Redirections (<, >, >>, <<) set redirection type and capture filename
- Regular word tokens are added as command arguments
*/
t_cmd_table *register_to_table(t_arena *arena, t_tokens *list_of_toks)
{
    t_cmd_table *table;
    t_tokens    *current_tok;
    t_cmd       *current_cmd;

    if (list_of_toks == NULL)
        return (NULL);
    current_tok = list_of_toks;    
    table = ar_alloc(arena, sizeof(t_cmd_table));
    if (!table)
        return (NULL);
    current_cmd = new_cmd_alloc(arena);
    if (!current_cmd)
    {
        printf("Memory alloc failed for t_cmd\n");
        // No need to clean up as it's in the arena
        return (NULL);
    }
    table->list_of_cmds = current_cmd;
    table->cmd_count = 1;

    while (current_tok)
    {
        if (current_tok->type == TOKEN_PIPE)
        {
            if (!current_cmd->cmd_av || !current_tok->next)
            {
                ft_printf("Syntax error around pipe\n");
                // No need to clean up as it's in the arena
                return (NULL);
            }
            current_cmd->next_cmd = new_cmd_alloc(arena);
            if (!current_cmd->next_cmd)
            {
                ft_printf("Memory allocation failed for new command\n");
                // No need to clean up as it's in the arena
                return (NULL);
            }
            current_cmd = current_cmd->next_cmd; //prev cmd done, onto new cmd
            table->cmd_count++;
        }
        else if (is_redirection(current_tok->type))
        {
            if (!current_tok->next || current_tok->next->type != TOKEN_WORD)
            {
                ft_printf("No valid name for redirection file\n");
                // No need to clean up as it's in the arena
                return (NULL);
            }
            if (make_redir(arena, current_tok, current_cmd) == -1)
            {
                // No need to clean up as it's in the arena
                return (NULL);
            }
        }
        else
        {
            if (current_tok->type != TOKEN_WORD)
                return(err_msg_n_return_null("Not a word token\n"));
            if (ft_strchr(current_tok->value, '&') && ft_strlen(current_tok->value) == 1)
                return (err_msg_n_return_null("Syntax error (lonely &)\n"));
            if (ft_strchr(current_tok->value, '$'))
                if (expand_variable_name(arena, current_tok) == -1)
                    return (NULL);
            if (add_argv(arena, current_cmd, current_tok->value) == -1)
                return (NULL);
        }
        current_tok = current_tok->next; //move to the next token
    }
    
    print_cmd_table(table);
    return (table);
}

/* Create space with ar_alloc() for a t_cmd struct
*/
t_cmd   *new_cmd_alloc(t_arena *arena)
{
    t_cmd   *new;

    new = ar_alloc(arena, sizeof(t_cmd));
    if (!new)
        return (NULL);
    new->cmd_av = NULL;
    new->redirections = NULL;
    new->next_cmd = NULL;
    return (new);
}

/* Expand command array to include a new token value:
 - Allocating new array with space for existing arguments + new argument + NULL
 - Give existing string pointers to new array
 - Use ar_strdup() to duplicate the new token value into the array
 - Add NULL terminator to the expanded array
 - Update the old command array with the new allocation

 Returns: 0 on success, -1 on errors
*/
int    add_argv(t_arena *arena, t_cmd *command, char *expansion)
{
    size_t  quantity;
    size_t  i;
    char    **new_cmd;

    quantity = 0;
    if (command->cmd_av)
        while (command->cmd_av[quantity])
            quantity++;
    new_cmd = ar_alloc(arena, (quantity + 2) * sizeof(char *));
    if (!new_cmd)
    {
        ft_putstr_fd("Allocation for command failed\n", 2);
        return (-1);
    }
    i = 0;
    if (command->cmd_av)
        while (command->cmd_av[i])
        {
            new_cmd[i] = command->cmd_av[i];
            i++;
        }
    new_cmd[i] = ar_strdup(arena, expansion);
    if (!new_cmd[i])
    {
        ft_putstr_fd("strdup fail while building command\n", 2);
        return (-1);
    }
    new_cmd[i + 1] = NULL;
    command->cmd_av = new_cmd;
    return (0);
}


/* Remove quotes and expand variable name
- Iterate through the token value to check each character
- Enter single/double quote mode and exit only when seeing the same quote
- Variable expansion according to the mode (in single quote or other modes)
- Other characters are added to the final string
*/
int    expand_variable_name(t_arena *arena, t_tokens *word_tok)
{
    size_t  i;
    int     in_2xquote;
    int     in_1xquote;
    char    *var_name;
    char    *expanded_text;

    i = 0;
    in_2xquote = 0;
    in_1xquote = 0;
    var_name = NULL;
    expanded_text = NULL;

    while (word_tok->value[i])
    {
        if (word_tok->value[i] == '$')
        {
            if (in_1xquote)
            {
                expanded_text = ar_add_char_to_str(arena, expanded_text, word_tok->value[i]);
                if (!expanded_text)
                    break ;
            }
            else
            {
                while (ft_isalnum(word_tok->value[i + 1]) || word_tok->value[i + 1] == '_')
                {
                    var_name = ar_add_char_to_str(arena, var_name, word_tok->value[i + 1]);
                    if (!var_name)
                        return (-1);
                    i++;
                }
                if (var_name)
                {
                    var_name = find_var_value(var_name);
                    expanded_text = ar_strjoin(arena, expanded_text, var_name);
                    if (!expanded_text)
                        return (-1);
                }
                else
                    expanded_text = ar_add_char_to_str(arena, expanded_text, '$');
            }
        }
        else if ((word_tok->value[i] == '\"' || word_tok->value[i] == '\'')
            && (!(in_2xquote || in_1xquote)))
        {
            if (word_tok->value[i] == '\"')
                in_2xquote = 1;
            if (word_tok->value[i] == '\'')
                in_1xquote = 1;
        }
        else if (word_tok->value[i] == '\"' && in_2xquote)
            in_2xquote = 0;
        else if (word_tok->value[i] == '\'' && in_1xquote)
            in_1xquote = 0;
        else
        {
            expanded_text = ar_add_char_to_str(arena, expanded_text, word_tok->value[i]);
            if (!expanded_text)
                break ;
        }
        i++;
    }
    if (!expanded_text)
        return (-1);
    word_tok->value = expanded_text;
    return (0);
}

/* Return the value of the variable name passed to the function,
or NULL if there is no such variable name in the system
(helper function of expand_variable_name)
*/
char    *find_var_value(char *name)
{
    char    *value;
    size_t  i;

    if (ft_isdigit(name[0]))
        return (err_msg_n_return_null("Bad environment name - starts with digit\n"));
    // if (name[0] == '?')
    // {
    //     if (name[1])
    //         return (err_msg_n_return_null("Bad environment name - more character after ?\n"));
    //     // get exit_status of the most recently executed foreground pipeline
    //     // value = ft_itoa(exit_status);
    //     // return (value);
    // }
    i = 0;
    while (name[i])
    {
        if (!(ft_isalnum(name[i]) || name[i] == '_'))
            return (err_msg_n_return_null("Bad environment name - forbidden characters\n"));
        i++;
    }
    value = getenv(name);
    if (!value)
        return(NULL);
    return (value);
}

void    *err_msg_n_return_null(char *msg)
{
    if (msg)
        ft_putstr_fd(msg, 2);
    return (NULL);
}
