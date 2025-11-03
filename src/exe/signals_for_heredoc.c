/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_for_heredoc.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:44:57 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/03 17:45:05 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
** handle_heredoc_sigint - Handler for Ctrl+C during heredoc input
**
** BEHAVIOR:
**   Sets the global signal flag. The rl_event_hook will detect this
**   and set rl_done to interrupt readline().
**
** WHY DIFFERENT FROM handle_sigint:
**   During heredoc, we DON'T want to redisplay the prompt.
**   We just want to cancel the heredoc and return to main prompt.
**   The rl_event_hook mechanism cleanly interrupts readline().
**
** EXIT CODE:
**   Heredoc interruption should result in exit status 130.
**
**   sig - Signal number (SIGINT = 2)
*/
void	handle_heredoc_sigint(int signum)
{
	g_signal = signum;
}

/**
** heredoc_event_hook - Readline event hook for heredoc signal checking
**
** DESCRIPTION:
**   This function is called periodically by readline() when set as
**   rl_event_hook. It checks if SIGINT was received and interrupts
**   readline by setting rl_done to 1.
**
** TECHNICAL DETAILS:
**   rl_event_hook is called between characters during readline().
**   It's safe to check our global variable here and interrupt readline.
**   Setting rl_done = 1 causes readline() to return NULL immediately.
**
** RETURN VALUE:
**   Always returns 0 (required by readline)
*/
int	heredoc_event_hook(void)
{
	if (g_signal)
		rl_done = 1;
	return (0);
}

/**
** setup_heredoc_signals - Configure signals for heredoc mode
**
** DESCRIPTION:
**   Sets up signal handlers and readline hook for heredoc input.
**   - SIGINT uses special handler that doesn't redisplay prompt
**   - SIGQUIT is ignored (standard bash behavior)
**   - rl_event_hook is set to check for signals periodically
**
** IMPORTANT:
**   Must call restore_interactive_signals() after heredoc completes
**   to restore normal signal handling!
*/
void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = &handle_heredoc_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
	rl_event_hook = &heredoc_event_hook;
	g_signal = 0;
}

/* handle_heredoc_interrupt - Handle Ctrl+C during heredoc input
** Sets exit status to 130 and returns 1.
*/
int	handle_heredoc_interrupt(t_shell *shell, char *line)
{
	if (line)
		free(line);
	restore_interactive_signals();
	shell->last_exit_status = 130;
	return (1);
}

// for main.c
// handle_signal_status - Update shell exit status based on signal
void	handle_signal_status(t_shell *shell)
{
	if (g_signal == SIGINT)
	{
		shell->last_exit_status = 130;
		g_signal = 0;
	}
}
