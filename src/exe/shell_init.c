#include "minishell.h"

//#define INITIAL_ENV_CAPACITY 128

// Static function prototypes
static int	init_shell_env(t_shell *shell, char **env);
static int	init_shell_variables(t_shell *shell, char **av);
static void	init_shell_paths(t_shell *shell);
static void	init_shell_user_info(t_shell *shell);

/**
** init_shell - Initialize shell state structure
**
**   Creates and initializes a new shell state with environment copy
**   and essential shell variables.
**
**   ac  - Argument count from main()
**   av  - Argument vector from main()
**   env - Environment variables from main()
**
**   Returns: Pointer to initialized t_shell structure, or NULL on failure
*/
t_shell	*init_shell(int ac, char **av, char **env)
{
	t_shell	*shell;

	shell = ft_calloc(1, sizeof(t_shell));
	if (!shell)
		return (NULL);
	if (!init_shell_env(shell, env))
		return (free_shell(shell), NULL);
	if (!init_shell_variables(shell, av))
		return (free_shell(shell), NULL);
	shell->last_exit_status = 0;
	shell->is_interactive = isatty(STDIN_FILENO);
	shell->should_exit = 0;
	shell->arena = ar_init();
	if (!shell->arena)
	{
		ft_printf("Failed to initialize memory arena\n");
		free_shell(shell);
		return (NULL);
	}
	(void)ac;
	return (shell);
}

/**
** init_shell_env - Initialize shell environment copy
**
**   Creates a dynamic copy of the environment variables with room for growth.
**
**   shell - Shell state structure
**   env   - Original environment variables
**
**   Returns: 1 on success, 0 on failure
*/
static int	init_shell_env(t_shell *shell, char **env)
{
	int	i;

	shell->env_count = 0;
	while (env[shell->env_count])
		shell->env_count++;
	shell->env_capacity = shell->env_count + 32;
	shell->env = ft_calloc(shell->env_capacity + 1, sizeof(char *));
	if (!shell->env)
		return (0);
	i = 0;
	while (i < shell->env_count)
	{
		shell->env[i] = ft_strdup(env[i]);
		if (!shell->env[i])
			return (free_partial_env(shell, i), 0);
		i++;
	}
	shell->env[i] = NULL;
	return (1);
}

/**
** init_shell_paths - Initialize shell path variables (CWD, OLDPWD, HOME)
**
**   shell - Shell state structure
*/
static void	init_shell_paths(t_shell *shell)
{
	char	*cwd_buffer;
	char	*env_value;

	cwd_buffer = getcwd(NULL, 0);
	if (cwd_buffer)
	{
		shell->cwd = ft_strdup(cwd_buffer);
		free(cwd_buffer);
	}
	else
		shell->cwd = ft_strdup("/");
	env_value = get_shell_env_value(shell, "OLDPWD");
	if (env_value)
		shell->oldpwd = ft_strdup(env_value);
	else
		shell->oldpwd = ft_strdup("");
	env_value = get_shell_env_value(shell, "HOME");
	if (env_value)
		shell->home = ft_strdup(env_value);
	else
		shell->home = ft_strdup("/");
}

/**
** init_shell_user_info - Initialize user and shell name variables
**
**   shell - Shell state structure
*/
static void	init_shell_user_info(t_shell *shell)
{
	char	*env_value;

	env_value = get_shell_env_value(shell, "USER");
	if (env_value)
		shell->user = ft_strdup(env_value);
	else
		shell->user = ft_strdup("unknown");
}

/**
** init_shell_variables - Initialize shell-specific variables
**
**   Sets up essential shell variables like PWD, HOME, USER, etc.
**
**   shell - Shell state structure
**   av    - Argument vector from main()
**
**   Returns: 1 on success, 0 on failure
*/
static int	init_shell_variables(t_shell *shell, char **av)
{
	init_shell_paths(shell);
	init_shell_user_info(shell);
	shell->shell_name = ft_strdup(av[0]);
	shell->shell_pid = getpid();
	if (!shell->cwd || !shell->oldpwd || !shell->home
		|| !shell->user || !shell->shell_name)
		return (0);
	return (1);
}
