/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wsm <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 09:33:30 by wsm               #+#    #+#             */
/*   Updated: 2025/11/04 09:33:35 by wsm              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** exit_shell - Clean up and exit the shell
**
**   shell     - Shell state to free
**   exit_code - Exit code to return
*/
static void	exit_shell(t_shell *shell, int exit_code)
{
	ft_printf("exit\n");
	rl_clear_history();
	free_shell(shell);
	exit(exit_code);
}

/*
** exit_invalid_arg - Handle invalid numeric argument for exit
**
**   shell - Shell state to free
**   arg   - Invalid argument string
*/
static void	exit_invalid_arg(t_shell *shell, char *arg)
{
	ft_printf("exit\n");
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
	rl_clear_history();
	free_shell(shell);
	exit(2);
}

/*
** builtin_exit - Implementation of exit command
**
** DESCRIPTION:
**   Exits the shell with optional exit code.
**   Validates arguments:
**   - Too many arguments: returns error without exiting
**   - Non-numeric argument: exits with code 2
**   - Valid number: exits with code % 256
**   Frees shell resources before exiting.
**
** RETURN VALUE:
**   Does not return on success - exits the program
**   Returns 1 if too many arguments
*/
int	builtin_exit(t_shell *shell, t_cmd *cmd)
{
	unsigned int	exit_code;
	long			long_code;

	if (!cmd || !cmd->cmd_av || !cmd->cmd_av[1])
		exit_shell(shell, 0);
	if (cmd->cmd_av[2])
	{
		print_error("minishell", "exit", "too many arguments");
		return (1);
	}
	if (!ft_isnumeric(cmd->cmd_av[1]))
		exit_invalid_arg(shell, cmd->cmd_av[1]);
	long_code = ft_atoi(cmd->cmd_av[1]);
	exit_code = (unsigned int)(long_code % 256);
	exit_shell(shell, exit_code);
	return (0);
}
