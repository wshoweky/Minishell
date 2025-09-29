//#include "../include/exe.h"
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
	int		i;
	int		newline;

	if (!cmd || !cmd->cmd_av)
		return (0);
	
	newline = 1;
	i = 1;
	
	// Check for -n flag
	if (cmd->cmd_av[1] && ft_strcmp(cmd->cmd_av[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
	}
	
	// Print arguments
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

	(void)cmd; // Suppress unused parameter warning
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
** builtin_cd - Implementation of cd command
**
** DESCRIPTION:
**   Changes current directory. Only supports single path argument.
**
** RETURN VALUE:
**   Returns 0 on success, 1 on error
*/
int	builtin_cd(t_cmd *cmd)
{
	char	*path;

	if (!cmd || !cmd->cmd_av)
		return (1);
	
	// cd with no arguments - go to HOME
	if (!cmd->cmd_av[1])
	{
		ft_printf("cd: no argument provided\n");
		return (1);
	}
	
	// cd with multiple arguments - error
	if (cmd->cmd_av[2])
	{
		ft_printf("cd: too many arguments\n");
		return (1);
	}
	
	path = cmd->cmd_av[1];
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}
	
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
int	builtin_env(char **env)
{
	int	i;

	if (!env)
		return (0);
	
	i = 0;
	while (env[i])
	{
		ft_printf("%s\n", env[i]);
		i++;
	}
	
	return (0);
}

/*
** builtin_exit - Implementation of exit command
**
** DESCRIPTION:
**   Exits the shell with optional exit code.
**
** RETURN VALUE:
**   Does not return - exits the program
*/
int	builtin_exit(t_cmd *cmd)
{
	int	exit_code;

	exit_code = 0;
	
	if (cmd && cmd->cmd_av && cmd->cmd_av[1])
		exit_code = ft_atoi(cmd->cmd_av[1]);
	
	ft_printf("exit\n");
	exit(exit_code);
}
