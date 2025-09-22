#include "minishell.h"

/* Check if token is a redirectional token.
*/
int is_redirection(t_token_type check)
{
    if (check == TOKEN_REDIRECT_IN || check == TOKEN_REDIRECT_OUT
        || check == TOKEN_APPEND || check == TOKEN_HEREDOC)
        return (8);
    return (0);
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
        ft_printf("Memory alloc failed for t_redir\n");
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