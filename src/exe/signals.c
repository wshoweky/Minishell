/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wshoweky <wshoweky@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 17:44:36 by wshoweky          #+#    #+#             */
/*   Updated: 2025/11/03 17:44:38 by wshoweky         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

/**
** disable_echoctl - Disable echoing of control characters
**
** DESCRIPTION:
**   Disables the terminal's automatic echoing of control characters
**   like ^C and ^\ to keep the prompt clean.
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	disable_echoctl(void)
{
	struct termios	terminal;

	if (tcgetattr(STDIN_FILENO, &terminal) == -1)
		return (-1);
	terminal.c_lflag &= ~ECHOCTL;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal) == -1)
		return (-1);
	return (0);
}

/**
** reset_signals_for_child - Reset signals to default for child processes
**
** DESCRIPTION:
**   Child processes should receive signals normally (not handled by shell).
**   Called after fork() but before execve().
*/
void	reset_signals_for_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
** restore_interactive_signals - Restore signals after heredoc
**
** DESCRIPTION:
**   Restores normal interactive signal handling after heredoc completes.
**   Clears the readline event hook and resets the signal flag.
**
** WHY NEEDED:
**   Heredoc mode uses special signal handling. After heredoc completes
**   (or is interrupted), we must restore normal interactive handlers.
*/
void	restore_interactive_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = &handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	sigaction(SIGQUIT, &sa, NULL);
	rl_event_hook = NULL;
	g_signal = 0;
}

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
void	handle_sigint(int signum)
{
	g_signal = signum;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
** setup_signal_handlers - Setup signal handlers for interactive mode
**
** DESCRIPTION:
**   Configures SIGINT and SIGQUIT handlers for the main shell loop.
**   Uses sigaction for more reliable signal handling than signal().
**   Also disables terminal echoing of control characters.
**
** RETURN VALUE:
**   Returns 0 on success, -1 on error
*/
int	setup_signal_handlers(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = &handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
		return (-1);
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
		return (-1);
	return (0);
}
