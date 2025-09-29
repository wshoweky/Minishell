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
            printf("Redirection no %i\n", current_cmd->redirections->tok_type);
            printf("filename saved: %s\n", current_cmd->redirections->filename);
            while (current_cmd->redirections->next)
            {
                current_cmd->redirections = current_cmd->redirections->next;
                printf("Redirection no %i\n", current_cmd->redirections->tok_type);
                printf("filename saved: %s\n", current_cmd->redirections->filename);
            }
        }
        current_cmd = current_cmd->next_cmd;
        cmd_index++;
    }
}


/*
- Free and assign as NULL each element in the string array
- Free the double pointer itself
- Return NULL
*/
char    **clean_free_double_pointers(char **trash)
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
    }
    return (NULL);
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

/* Check if token is a redirectional token.
*/
int is_redirection(t_token_type check)
{
    if (check == TOKEN_REDIRECT_IN || check == TOKEN_REDIRECT_OUT
        || check == TOKEN_APPEND || check == TOKEN_HEREDOC)
        return (8);
    return (0);
}

/* Create space with ar_alloc for a t_cmd struct so all values are 0
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

/* Create and append a new redirection node to the command

- Allocate memory for a new t_redir structure
- Set redirection type from current token
- Move to next token and ar_strdup the filename
- Appends the new t_redir to the command's redirection list.
    If no redirection exists, it becomes the first node; otherwise at the end.

Returns: 0 on success, -1 on errors
*/
int    make_redir(t_arena *arena, t_tokens *curr_tok, t_cmd *curr_cmd)
{
    t_redir *new;
    t_redir *find_tail;

    new = ar_alloc(arena, sizeof(t_redir));
    if (!new)
    {
        ft_printf("Memory alloc failed for t_redir");
        return (-1);
    }
    set_redir_type(curr_tok->type, &new->tok_type);
    new->next = NULL;
    *curr_tok = *curr_tok->next;
    new->filename = ar_strdup(arena, curr_tok->value);
    if (!new->filename)
    {
        ft_printf("Memory allocation failed for file name\n");
        return (-1);
    }
    if (!curr_cmd->redirections)
        curr_cmd->redirections = new;
    else
    {
        find_tail = curr_cmd->redirections;
        while (find_tail->next)
            find_tail = find_tail->next;
        find_tail->next = new;
    }
    return (0);
}

/* Set the redirectional type to be the same with the token type
(helper function of make_redir())
*/
void    set_redir_type(t_token_type tok_type, t_token_type *redir_type)
{
    if (tok_type == TOKEN_REDIRECT_IN)
        *redir_type = TOKEN_REDIRECT_IN;
    else if (tok_type == TOKEN_REDIRECT_OUT)
        *redir_type = TOKEN_REDIRECT_OUT;
    else if (tok_type == TOKEN_APPEND)
        *redir_type = TOKEN_APPEND;
    else if (tok_type == TOKEN_HEREDOC)
        *redir_type = TOKEN_HEREDOC;
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
            {
                ft_printf("Not a word token\n");
                // No need to clean up as it's in the arena
            if (ft_strchr(current_tok->value, '&') && ft_strlen(current_tok->value) == 1)
                return (err_msg_n_return_null("Syntax error (lonely &)\n"));
            if (ft_strchr(current_tok->value, '$'))
                if (expand_variable_name(arena, current_tok) == -1)
                    return (NULL);
            if (add_argv(arena, current_cmd, current_tok->value) == -1)
                return (NULL);
            }
            add_argv(arena, current_cmd, current_tok->value);
            if (!current_cmd->cmd_av)
            {
                ft_printf("Adding command argv unsuccessful\n");
                // No need to clean up as it's in the arena
                return (NULL);
            }
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


/* Early stage variable name expansion
*/
int    expand_variable_name(t_arena *arena, t_tokens *word_tok)
{
    char    *var_name;
    char    *var_value;
    char    *normal_word;
    size_t  normal_word_len;
    size_t  i;

    if (word_tok->was_quoted == 0)
    {
        i = 0;
        var_name = ft_strchr(word_tok->value, '$');
        normal_word = NULL;
        normal_word_len = ft_strlen(word_tok->value) - ft_strlen(var_name);
        if (normal_word_len > 0)
        {
            normal_word = ar_substr(arena, word_tok->value, 0, normal_word_len);
            if (!normal_word)
            {
                ft_putstr_fd("Allocation for normal text failed \n", 2);
                return (-1);
            }
        }
        var_value = find_var_value(var_name + 1); //skip the $
        word_tok->value = ar_strjoin(arena, normal_word, var_value);
        if (!word_tok->value)
        {
            ft_putstr_fd("Cannot update token value after variable expansion\n", 2);
            return (-1);
        }
        return (0);
    }
    // else if (word_tok->was_quoted == 1)
    // {
    //     if (expand_var_name_1quotes(arena, word_tok) == -1)
    //     {
    //         ft_putstr_fd("Error in expanding variable name in single quotes\n", 2);
    //         return (-1);
    //     }
    //     return (0);
    // }
    else
    {
        if (expand_var_name_2quotes(arena, word_tok) == -1)
        {
            ft_putstr_fd("Error in expanding variable name in double quotes\n", 2);
            return (-1);
        }
        return (0);
    }
}

int expand_var_name_2quotes(t_arena *arena, t_tokens *word_tok)
{
    size_t  i;
    char    *plain_text;
    char    *var_name;
    size_t  var_len;

    i = 0;
    plain_text = NULL;
    var_name = NULL;
    var_len = 0;
    
    while (word_tok->value[i])
    {
        if (word_tok->value[i] == '$')
        {
            i++; //skip $ sign
            while (ft_isalnum(word_tok->value[i]) && word_tok->value[i] != '_')
            {
                var_name = ar_add_char_to_str(arena, var_name, word_tok->value[i]);
                var_len++;
                i++;
            }
            ft_printf("varlen = %i; varname = %s\n", var_len, var_name);
            if (var_len)
            {
                ft_printf("varlen = %i; varname = %s\n", var_len, var_name);
                var_name = find_var_value(var_name);
                plain_text = ar_strjoin(arena, plain_text, var_name);
                if (!plain_text)
                    return (-1);
            }
            else
                plain_text = ar_strjoin(arena, plain_text, "$");
        }
        if (word_tok->value[i] != '"')
            plain_text = ar_add_char_to_str(arena, plain_text, word_tok->value[i]);
        i++;
    }ft_printf("Here expand 2quotes\n");
    word_tok->value = plain_text;
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
        if (!ft_isalnum(name[i]) && name[i] != '_')
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
