// #include "minishell.h"
// #include <termios.h> // For terminal attribute manipulation

// volatile sig_atomic_t	g_signal = 0; // Global variable to track signal state

// int	disable_echoctl(void)
// {
// 	struct termios	terminal;

// 	if (tcgetattr(STDIN_FILENO, &terminal) == -1) // Get current terminal attributes
// 		return (-1);
// 	terminal.c_lflag &= ~ECHOCTL; // Disable echoing of control characters
// 	if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal) == -1) // Apply changes immediately
// 		return (-1);
// 	return (0);
// }

// void	reset_signals_for_child(void)
// {
// 	signal(SIGINT, SIG_DFL);  // Reset SIGINT to default behavior
// 	signal(SIGQUIT, SIG_DFL); // Reset SIGQUIT to default behavior
// }

// void	restore_interactive_signals(void)
// {
// 	struct sigaction	sa;

// 	sa.sa_handler = handle_sigint; // Custom handler for SIGINT
// 	sigemptyset(&sa.sa_mask); // No additional signals blocked during handler
// 	sa.sa_flags = SA_RESTART; // Restart interrupted syscalls
// 	sigaction(SIGINT, &sa, NULL); // Apply SIGINT handler
// 	sa.sa_handler = SIG_IGN; // Ignore SIGQUIT
// 	sa.sa_flags = 0; // No special flags
// 	sigaction(SIGQUIT, &sa, NULL); // Apply SIGQUIT handler
// 	rl_event_hook = NULL; // Clear readline event hook
// 	g_signal = 0; // Reset global signal state
// }

// void	handle_sigint(int sig)
// {
// 	g_signal = sig; // Update global signal state
// 	write (STDOUT_FILENO, "\n", 1); // Move to new line
// 	rl_on_new_line(); // Inform readline of new line
// 	rl_replace_line("", 0); // Clear current input line
// 	rl_redisplay(); // Refresh the prompt
// }

// int	setup_signal_handlers(void)
// {
// 	struct sigaction	sa_int;
// 	struct sigaction	sa_quit;

// 	sa_int.sa_handler = handle_sigint; // Set custom handler for SIGINT
// 	sigemptyset (&sa_int.sa_mask); // No additional signals blocked during handler
// 	sa_int.sa_flags = SA_RESTART; // Restart interrupted syscalls
// 	if (sigaction(SIGINT, &sa_int, NULL) == -1) // Apply SIGINT handler
// 		return (-1);
// 	sa_quit.sa_handler = SIG_IGN; // Ignore SIGQUIT
// 	sigemptyset(&sa_quit.sa_mask); // No additional signals blocked during handler
// 	sa_quit.sa_flags = 0; // No special flags
// 	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1) // Apply SIGQUIT handler
// 		return (-1);
// 	return (0);
// }