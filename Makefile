NAME = minishell

CC = cc 


CFLAG = -Wall -Wextra -Werror #-g3 -O0 -ggdb3 #-fno-omit-frame-pointer -fsanitize=address

# Colors for output
RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
PURPLE = \033[35m
ORANGE = \033[38;5;208m
RESET = \033[0m


SRC = src/main.c \
	  src/parser/tokens.c \
	  src/parser/tokens_break_input.c \
	  src/parser/tokens_quote_check.c \
	  src/parser/token_list_utils.c \
	  src/parser/extract_tokens.c \
 	  src/parser/build_cmd_table.c \
	  src/parser/build_cmd_table_redir.c \
	  src/parser/build_cmd_table_word.c \
	  src/parser/build_cmdtbl_var_1.c \
	  src/parser/build_cmdtbl_var_2.c \
	  src/parser/arena.c \
	  src/parser/arena_utils.c \
	  src/parser/arena_split.c \
	  src/parser/arena_itoa.c \
	  src/exe/exe.c \
	  src/exe/exe_utils.c \
	  src/exe/redirection.c \
	  src/exe/builtins.c \
	  src/exe/builtin_export.c \
	  src/exe/builtin_export_parse.c \
	  src/exe/builtin_export_publish.c \
	  src/exe/builtin_unset.c \
	  src/exe/path_utils.c \
	  src/exe/shell_init.c \
	  src/exe/free_shell.c \
	  src/exe/shell_tools.c \
	  src/exe/shell_utils.c \
	  src/exe/pipeline.c \
	  src/exe/pipe_utils.c \
	  src/exe/heredoc.c \
	  src/exe/heredoc_utils.c \
	  src/exe/heredoc_delimiter_special.c \
	  src/exe/signals.c \
	  src/exe/signals_for_heredoc.c
	


O_DIR = objdir

OBJ = $(addprefix $(O_DIR)/,$(SRC:.c=.o))

LIBFT_DIR = ./libft

LIBFT = $(LIBFT_DIR)/libft.a

READLINE = -lreadline

all: $(NAME) $(LIBFT)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAG) $(OBJ) $(LIBFT) $(READLINE) -o $(NAME)
	@$(MAKE) success_message
	@$(MAKE) art


$(O_DIR):
	mkdir -p $(O_DIR)/src
	mkdir -p $(O_DIR)/src/parser
	mkdir -p $(O_DIR)/src/exe
#	mkdir -p $(O_DIR)/src/playground

$(O_DIR)/%.o: %.c include/minishell.h | $(O_DIR)
	$(CC) $(CFLAG) -I$(LIBFT_DIR) -Iinclude -c $< -o $@

clean:
	rm -rf $(O_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean
	@echo "\033[33m** Object files deleted **\033[0m"

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "\033[33m** Program deleted **\033[0m"

re: fclean all

# =============================================================================
#  VALGRIND
# =============================================================================
valgrind: re
	@echo "${PURPLE}🔍 Running memory and file descriptor analysis with valgrind...${RESET}"
	valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --suppressions=readline.supp --track-origins=yes ./$(NAME) 
# =============================================================================
#  MESSAGES
#      Custom messages displayed during the build process.
# =============================================================================
success_message:
	@echo "${ORANGE}🎃═══════════════════════════════════════════════════════════🎃"
	@echo "${ORANGE}                ✨ SPOOKY COMPILATION SUCCESS! ✨"
	@echo "${ORANGE}               👻 Your haunted shell is ready! 👻"
	@echo "${ORANGE}🎃═══════════════════════════════════════════════════════════🎃${RESET}"

art:
	@echo ""
	@echo "${ORANGE}        🎃════════════════════════════════════════════════════════🎃"
	@echo "${ORANGE}                ███████ ██████   ██████   ██████  ██   ██ ██    ██ "
	@echo "${ORANGE}                ██      ██   ██ ██    ██ ██    ██ ██  ██   ██  ██  "
	@echo "${ORANGE}                ███████ ██████  ██    ██ ██    ██ █████      ████   "
	@echo "${ORANGE}                     ██ ██      ██    ██ ██    ██ ██  ██      ██    "
	@echo "${ORANGE}                ███████ ██       ██████   ██████  ██   ██     ██    "
	@echo ""
	@echo "${ORANGE}                ███████ ██   ██ ███████ ██      ██      "
	@echo "${ORANGE}                ██      ██   ██ ██      ██      ██      "
	@echo "${ORANGE}                ███████ ███████ █████   ██      ██      "
	@echo "${ORANGE}                     ██ ██   ██ ██      ██      ██      "
	@echo "${ORANGE}                ███████ ██   ██ ███████ ███████ ███████ "
	@echo "${ORANGE}        🎃════════════════════════════════════════════════════════🎃"
	@echo ""
	@echo "${ORANGE} 	                👻 ${RED}Boo-tiful Shell!${ORANGE} 👻"
	@echo "${ORANGE}                   🦇 ${RED}Halloween Edition 2025${ORANGE} 🦇"
	@echo "${RESET}"

.PHONY: all clean fclean re valgrind success_message art

.SECONDARY: $(OBJ)

.SILENT:
