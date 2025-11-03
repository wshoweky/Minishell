#include "minishell.h"

static int	find_last_valid_cmd(t_shell *shell, int cmd_count);
static void	process_child_exit_status(t_shell *shell, int status);

/**
** alloc_pipe_array - Allocate array of pipe file descriptors
**
** PIPE ARRAY STRUCTURE:
** For N commands, we need (N-1) pipes:
** - pipe_array[0]: connects cmd[0] → cmd[1]
** - pipe_array[1]: connects cmd[1] → cmd[2]
** - pipe_array[i]: connects cmd[i] → cmd[i+1]
**
** Each pipe_array[i] contains: [read_fd, write_fd]
**
** WHY ARRAY OF ARRAYS: Direct indexing allows O(1) access to any pipe.
** Alternative linked list would require O(n) traversal.
**
**   shell     - Shell state with arena
**   cmd_count - Number of commands (needs cmd_count-1 pipes)
**
**   Returns: Allocated pipe array or NULL on failure
*/
int	**alloc_pipe_array(t_shell *shell, int cmd_count)
{
	int	pipes_needed;
	int	**pipe_array;
	int	i;

	pipes_needed = cmd_count - 1;
	if (pipes_needed <= 0)
		return (NULL);
	pipe_array = ar_alloc(shell->arena, sizeof(int *) * pipes_needed);
	if (!pipe_array)
		return (NULL);
	i = 0;
	while (i < pipes_needed)
	{
		pipe_array[i] = ar_alloc(shell->arena, sizeof(int) * 2);
		if (!pipe_array[i])
			return (NULL);
		i++;
	}
	return (pipe_array);
}

/**
 * close_unused_pipes - Close pipe file descriptors
 *
 * USAGE SCENARIOS:
 * 1. Child process (after dup2): Close ALL pipes (pipes_to_close = cmd_count
	- 1)
 *    - Safe because dup2() already copied needed FDs to stdin/stdout
	* 2. Parent on error: Close only successfully created pipes
 *    - Prevents FD leaks when pipe creation/fork fails midway
 *
 * WHY CLOSE ALL IN CHILD: Child processes inherit ALL pipe FDs from parent.
 * Keeping unnecessary FDs open causes:
 * 1. File descriptor leaks
 * 2. Pipes not closing when they should (writers still exist)
 * 3. Deadlocks (readers waiting for data from pipes with extra writers)
 *
 * @shell: Shell state with pipe arrays
 * @pipes_to_close: Number of pipes to close (0 to pipes_to_close-1)
 */
void	close_unused_pipes(t_shell *shell, int pipes_to_close)
{
	int	i;

	i = 0;
	while (i < pipes_to_close)
	{
		close(shell->pipe_array[i][0]);
		close(shell->pipe_array[i][1]);
		i++;
	}
}

/**
** wait_all_children - Wait for all pipeline processes to complete
**
** WAITING STRATEGY:
** - Must wait for ALL children to prevent zombie processes
** - Children may exit in any order, but we wait sequentially (0→N)
** - Only the LAST command's exit status matters (bash behavior)
** - waitpid() blocks if child still running,
**	returns immediately if already exited
**
** WHY SEQUENTIAL WAIT: Simple, deterministic,
**	and easy to identify last command.
** Waiting order doesn't affect correctness - only matters that we wait for ALL.
**
**   shell     - Shell state with pipe PIDs
**   cmd_count - Number of child processes to wait for
*/
void	wait_all_children(t_shell *shell, int cmd_count)
{
	int					i;
	int					status;
	int					last_valid_cmd_index;
	struct sigaction	sa_ignore;
	struct sigaction	sa_old;

	sa_ignore.sa_handler = SIG_IGN;
	sigemptyset(&sa_ignore.sa_mask);
	sa_ignore.sa_flags = 0;
	sigaction(SIGINT, &sa_ignore, &sa_old);
	last_valid_cmd_index = find_last_valid_cmd(shell, cmd_count);
	i = 0;
	while (i < cmd_count)
	{
		if (shell->pipe_pids[i] > 0)
		{
			waitpid(shell->pipe_pids[i], &status, 0);
			if (i == last_valid_cmd_index)
				process_child_exit_status(shell, status);
		}
		i++;
	}
	sigaction(SIGINT, &sa_old, NULL);
}

/**
** find_last_valid_cmd - Find index of last successfully forked command
**
**   shell     - Shell state with pipe PIDs
**   cmd_count - Number of commands
**
**   Returns: Index of last valid command, or -1 if none found
*/
static int	find_last_valid_cmd(t_shell *shell, int cmd_count)
{
	int	i;

	i = cmd_count - 1;
	while (i >= 0)
	{
		if (shell->pipe_pids[i] > 0)
			return (i);
		i--;
	}
	return (-1);
}

/**
** process_child_exit_status - Extract and store exit status from child
**
** Called only for the last command in pipeline to get final exit status.
**
**   shell  - Shell state to update
**   status - Wait status from waitpid
*/
static void	process_child_exit_status(t_shell *shell, int status)
{
	if (WIFEXITED(status))
		shell->last_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		// if (WTERMSIG(status) == SIGINT)
		// 	write(STDOUT_FILENO, "\n", 1);
		shell->last_exit_status = 128 + WTERMSIG(status);
	}
	else
		shell->last_exit_status = 1;
}
