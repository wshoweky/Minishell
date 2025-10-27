#include "minishell.h"

/*
** PIPELINE ARCHITECTURE EXPLANATION:
**
** Why Arrays Instead of Linked Lists?
** ==================================
**
** 1. RANDOM ACCESS: Pipeline execution requires O(1) access by command index
**    - Command i needs pipe[i-1] for input and pipe[i] for output
**    - Linked lists would require O(n) traversal for each access
**
** 2. INDEX-BASED LOGIC: Pipeline connections are fundamentally indexed:
**    - cmd[0]: stdin → pipe[0]
**    - cmd[1]: pipe[0] → pipe[1]
**    - cmd[2]: pipe[1] → pipe[2]
**    - cmd[n]: pipe[n-1] → stdout
**
** 3. MEMORY EFFICIENCY: Arrays provide better cache locality and fewer
**    allocations compared to scattered linked list nodes
**
** 4. ARENA INTEGRATION: Arrays work perfectly with arena allocation -
**    simple allocation and automatic cleanup on arena reset
*/

static int	init_pipeline(t_shell *shell, int cmd_count);
static void	execute_pipeline_loop(t_shell *shell, t_cmd_table *cmd_table);
static void	fork_pipeline_child(t_shell *shell, t_cmd *cmd, int i,
				int cmd_count);
static void	close_partial_pipes(t_shell *shell, int pipes_created);
static int	create_pipe_for_cmd(t_shell *shell, int i, int cmd_count);
static int	process_pipeline_cmd(t_shell *shell, t_cmd **cmd, int i,
				int cmd_count);

/**
** execute_pipeline - Execute a pipeline of commands
**
** PIPELINE EXECUTION FLOW:
** 1. Allocate pipe arrays and PID tracking arrays
** 2. Fork each command in sequence, setting up pipe connections
** 3. Close unused pipe ends in parent process
** 4. Wait for all children and collect exit status from last command
**
** NOTE: This function is only called when cmd_count > 1
** (single commands are handled by exe_single_cmd in exe.c)
**
**   shell     - Shell state with pipe arrays (will be populated)
**   cmd_table - Command table with linked list of commands
*/
void	execute_pipeline(t_shell *shell, t_cmd_table *cmd_table)
{
	int	cmd_count;

	if (!cmd_table || !cmd_table->list_of_cmds)
		return ;
	cmd_count = cmd_table->cmd_count;
	if (!init_pipeline(shell, cmd_count))
		return ;
	shell->children_forked = 0;
	execute_pipeline_loop(shell, cmd_table);
	if (shell->children_forked > 0)
		close_unused_pipes(shell, cmd_count, -1);
	if (shell->children_forked > 0)
		wait_all_children(shell, cmd_count);
}

/**
** init_pipeline - Initialize pipes and PID arrays for pipeline
**
** ARRAY ALLOCATION STRATEGY:
** - pipe_array: Array of (cmd_count-1) pipe pairs [read_fd, write_fd]
**   Example: 3 commands need 2 pipes: cmd1|cmd2|cmd3
** - pipe_pids: Array of cmd_count PIDs for tracking child processes
**
** WHY ARRAYS: We need random access by index for pipe connections.
** Each command i connects to pipe[i-1] (input) and pipe[i] (output).
**
**   shell     - Shell state structure
**   cmd_count - Number of commands in pipeline
**
**   Returns: 1 on success, 0 on failure
*/
static int	init_pipeline(t_shell *shell, int cmd_count)
{
	shell->pipe_array = alloc_pipe_array(shell, cmd_count);
	shell->pipe_pids = ar_alloc(shell->arena, sizeof(int) * cmd_count);
	if (!shell->pipe_pids || !shell->pipe_array)
	{
		write(STDERR_FILENO, "minishell: pipeline: memory allocation failed\n",
			47);
		shell->last_exit_status = 1;
		return (0);
	}
	return (1);
}

/**
** execute_pipeline_loop - Fork and execute each command in pipeline
**
** PIPELINE FORKING STRATEGY:
** For each command in the pipeline:
** 1. Create pipe for communication with next command (if not last)
** 2. Fork child process
** 3. Child: Set up pipe connections and execute command
** 4. Parent: Continue to next command
**
** WHY LOOP WITH INDEX: We need the command index for pipe connections.
** Command i reads from pipe[i-1] and writes to pipe[i].
**
**   shell     - Shell state structure
**   cmd_table - Command table with linked list of commands
*/
static void	execute_pipeline_loop(t_shell *shell, t_cmd_table *cmd_table)
{
	t_cmd	*current_cmd;
	int		cmd_count;
	int		i;

	current_cmd = cmd_table->list_of_cmds;
	cmd_count = cmd_table->cmd_count;
	i = 0;
	while (i < cmd_count && current_cmd)
	{
		if (process_pipeline_cmd(shell, &current_cmd, i, cmd_count) == -1)
			return ;
		i++;
	}
}

/**
** create_pipe_for_cmd - Create pipe for command if not last in pipeline
**
**   shell     - Shell state structure
**   i         - Current command index
**   cmd_count - Total number of commands
**
**   Returns: 0 on success, -1 on error
*/
static int	create_pipe_for_cmd(t_shell *shell, int i, int cmd_count)
{
	if (i < cmd_count - 1)
	{
		if (pipe(shell->pipe_array[i]) < 0)
		{
			perror("minishell: pipe");
			shell->last_exit_status = 1;
			close_partial_pipes(shell, i);
			return (-1);
		}
	}
	return (0);
}

/**
** process_pipeline_cmd - Process single command in pipeline
**
**   shell     - Shell state structure
**   cmd       - Pointer to current command (will be advanced)
**   i         - Current command index
**   cmd_count - Total number of commands
**
**   Returns: 0 on success, -1 on error
*/
static int	process_pipeline_cmd(t_shell *shell, t_cmd **cmd, int i,
		int cmd_count)
{
	if (create_pipe_for_cmd(shell, i, cmd_count) == -1)
		return (-1);
	fork_pipeline_child(shell, *cmd, i, cmd_count);
	if (shell->pipe_pids[i] < 0)
	{
		close_partial_pipes(shell, i + 1);
		return (-1);
	}
	*cmd = (*cmd)->next_cmd;
	return (0);
}

/**
** fork_pipeline_child - Fork and setup child process for pipeline command
**
** CHILD PROCESS SETUP:
** 1. Redirect stdin/stdout to appropriate pipes based on command position
** 2. Close all unused pipe file descriptors (prevent deadlocks)
** 3. Execute the command (builtin or external)
** 4. Exit with command's exit status
**
** PARENT PROCESS: Just stores the child PID for later waiting
**
**   shell     - Shell state structure
**   cmd       - Command to execute
**   i         - Command index in pipeline (0-based)
**   cmd_count - Total number of commands
*/
static void	fork_pipeline_child(t_shell *shell, t_cmd *cmd, int i,
		int cmd_count)
{
	shell->pipe_pids[i] = fork();
	if (shell->pipe_pids[i] == 0)
	{
		setup_pipe_fds(shell, i, cmd_count);
		close_unused_pipes(shell, cmd_count, i);
		exe_single_cmd(shell, cmd);
		exit(shell->last_exit_status);
	}
	else if (shell->pipe_pids[i] < 0)
	{
		perror("minishell: fork");
		shell->last_exit_status = 1;
	}
	else
		shell->children_forked++;
}

/**
** close_partial_pipes - Close pipes created before pipeline failure
**
** CLEANUP STRATEGY:
** When pipeline creation fails (pipe() or fork() error), we need to close
** any pipes that were successfully created in previous loop iterations.
** This prevents file descriptor leaks.
**
**   shell         - Shell state with pipe arrays
**   pipes_created - Number of pipes successfully created (0-based)
*/
static void	close_partial_pipes(t_shell *shell, int pipes_created)
{
	int	i;

	i = 0;
	while (i < pipes_created)
	{
		close(shell->pipe_array[i][0]);
		close(shell->pipe_array[i][1]);
		i++;
	}
}
