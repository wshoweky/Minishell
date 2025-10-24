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
** setup_pipe_fds - Setup input/output redirection for pipeline command
**
** PIPE CONNECTION LOGIC:
** - Command 0: stdin → pipe[0]        (only output pipe)
** - Command i: pipe[i-1] → pipe[i]    (input and output pipes)
** - Command N: pipe[N-1] → stdout     (only input pipe)
**
** WHY INDEX-BASED: Each command's position determines which pipes to use.
** This is why arrays are perfect - direct calculation of pipe indices.
**
**   shell     - Shell state with pipe arrays
**   cmd_index - Current command index (0-based)
**   cmd_count - Total number of commands
*/
void	setup_pipe_fds(t_shell *shell, int cmd_index, int cmd_count)
{
	/* Connect stdin to previous command's output pipe (except first command) */
	if (cmd_index > 0)
	{
		/* Read from pipe[cmd_index-1] - previous command writes here */
		dup2(shell->pipe_array[cmd_index - 1][0], STDIN_FILENO);
	}
	/* Connect stdout to next command's input pipe (except last command) */
	if (cmd_index < cmd_count - 1)
	{
		/* Write to pipe[cmd_index] - next command reads from here */
		dup2(shell->pipe_array[cmd_index][1], STDOUT_FILENO);
	}
}

/**
 * close_unused_pipes - Close pipe file descriptors not needed by process
 *
 * Pipeline structure: cmd0 -> pipe[0] -> cmd1 -> pipe[1] -> cmd2 -> ...
 * Each command needs only specific pipe ends based on its position:
 * - cmd[i] needs pipe[i-1] for reading (input) if i > 0
 * - cmd[i] needs pipe[i] for writing (output) if not last command
 *
 * @shell: Shell state with pipe arrays
 * @cmd_count: Total number of commands
 * @current_cmd: Current command index (-1 for parent process)
 */
void	close_unused_pipes(t_shell *shell, int cmd_count, int current_cmd)
{
	int	i;
	int	pipes_needed;

	i = 0;
	pipes_needed = cmd_count - 1;
	while (i < pipes_needed)
	{
		if (current_cmd == -1 || (current_cmd != i && current_cmd != i + 1))
		{
			close(shell->pipe_array[i][0]);
			close(shell->pipe_array[i][1]);
		}
		else
		{
			if (current_cmd == i + 1)
				close(shell->pipe_array[i][1]);
			if (current_cmd == i)
				close(shell->pipe_array[i][0]);
		}
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
	int	i;
	int	status;
	int	last_valid_cmd_index;

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
		shell->last_exit_status = 128 + WTERMSIG(status);
	else
		shell->last_exit_status = 1;
}
