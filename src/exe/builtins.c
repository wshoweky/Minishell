/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:38:04 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/04 09:28:04 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** builtin_echo - Implementation of echo command
**
** DESCRIPTION:
**   Prints arguments separated by spaces, with optional -n flag.
**
** PARAMETERS:
**   cmd - Command structure with arguments
**
** RETURN VALUE:
**   Returns 0 on success
*/
int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	if (!cmd || !cmd->cmd_av)
		return (0);
	newline = 1;
	i = 1;
	while (cmd->cmd_av[i] && cmd->cmd_av[i][0] == '-'
		&& cmd->cmd_av[i][1] == 'n')
	{
		if (nl_flag_acceptable(cmd->cmd_av[i], &newline) == -1)
			break ;
		i++;
	}
	while (cmd->cmd_av[i])
	{
		ft_printf("%s", cmd->cmd_av[i]);
		if (cmd->cmd_av[i + 1])
			ft_printf(" ");
		i++;
	}
	if (newline)
		ft_printf("\n");
	return (0);
}

/*Check if following -n, argument either ends or only contains 'n' character
to determine if argument is a valid no-new-line flag or not

Return: 0 for valid flag, -1 for invalid
*/
int	nl_flag_acceptable(char *cmd_av, int *newline)
{
	int	j;

	j = 2;
	while (cmd_av[j])
	{
		if (cmd_av[j] != 'n')
			return (-1);
		j++;
	}
	*newline = 0;
	return (0);
}

/*
** builtin_pwd - Implementation of pwd command
**
** DESCRIPTION:
**   Prints current working directory.
**
** RETURN VALUE:
**   Returns 0 on success, 1 on error
*/
int	builtin_pwd(t_cmd *cmd)
{
	char	*cwd;

	(void)cmd;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: pwd");
		return (1);
	}
	ft_printf("%s\n", cwd);
	free(cwd);
	return (0);
}

/*
** builtin_env - Implementation of env command
**
** DESCRIPTION:
**   Prints all environment variables.
**
** RETURN VALUE:
**   Returns 0 on success
*/
int	builtin_env(t_shell *shell)
{
	int	i;

	if (!shell || !shell->env)
		return (0);
	i = 0;
	while (shell->env[i])
	{
		ft_printf("%s\n", shell->env[i]);
		i++;
	}
	return (0);
}
