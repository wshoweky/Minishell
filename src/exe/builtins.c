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

/*
** builtin_exit - Implementation of exit command
**
** DESCRIPTION:
**   Exits the shell with optional exit code.
**   Validates arguments:
**   - Too many arguments: returns error without exiting
**   - Non-numeric argument: exits with code 2
**   - Valid number: exits with code % 256
**
** RETURN VALUE:
**   Does not return on success - exits the program
**   Returns 1 if too many arguments
*/
int	builtin_exit(t_cmd *cmd)
{
	unsigned int	exit_code;
	long			long_code;

	if (!cmd || !cmd->cmd_av || !cmd->cmd_av[1])
	{
		ft_printf("exit\n");
		exit(0);
	}
	if (cmd->cmd_av[2])
	{
		ft_printf("minishell: exit: too many arguments\n");
		return (1);
	}
	if (!ft_isnumeric(cmd->cmd_av[1]))
	{
		ft_printf("exit\n");
		ft_printf("minishell: exit: %s: numeric argument required\n",
			cmd->cmd_av[1]);
		exit(2);
	}
	long_code = ft_atoi(cmd->cmd_av[1]);
	exit_code = (unsigned int)(long_code % 256);
	ft_printf("exit\n");
	exit(exit_code);
}
