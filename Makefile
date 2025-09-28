NAME = minishell

CC = cc 


CFLAG = -Wall -Wextra -Werror #-g3 -O0 -ggdb3 -fno-omit-frame-pointer -fsanitize=address

# Colors for output
RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
PURPLE = \033[35m
ORANGE = \033[38;5;208m
RESET = \033[0m


SRC = src/main.c \
	  src/parser/tokens.c \
	  src/parser/tokens_utils.c \
	  src/parser/token_list_utils.c \
	  src/parser/extract_tokens.c \
 	  src/parser/build_cmd_table.c \
	  src/parser/arena.c \
	  src/parser/arena_utils.c \
	  src/exe/exe.c \
	  src/exe/builtins.c \
	  src/exe/path_utils.c \

	  src/playground/shelly.c

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
	mkdir -p $(O_DIR)/src/playground

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
#  VALGRIND AND STATIC ANALYSIS
# =============================================================================

# -----------------------------------------------------------------------------
# VALGRIND - Memory Error Detection Tool  
# -----------------------------------------------------------------------------
# What it does:
#   Valgrind detects memory leaks, buffer overflows, and other memory errors
#   by running your program in a virtual machine that tracks all memory operations.
#
# Why use it:
#   - Finds memory leaks (allocated but not freed memory)
#   - Detects buffer overflows and underflows
#   - Identifies use of uninitialized memory
#   - Tracks file descriptor leaks (unclosed files/pipes)
#   - Essential for C programs with manual memory management
#   - Critical for shell programs that handle pipes and redirections
#
# Flags explained:
#   --leak-check=full      : Detailed memory leak detection
#   --show-leak-kinds=definite : Show definite memory leaks
#   --track-origins=yes    : Track where uninitialized values come from
#   --track-fds=yes        : Track file descriptor leaks (unclosed files)
# -----------------------------------------------------------------------------
valgrind: re
	@echo "${PURPLE}🔍 Running memory and file descriptor analysis with valgrind...${RESET}"
	valgrind --leak-check=full --show-leak-kinds=definite --track-origins=yes --track-fds=yes ./$(NAME) 
# -----------------------------------------------------------------------------
# CPPCHECK - Static Code Analysis Tool
# -----------------------------------------------------------------------------
# What it does:
#   Cppcheck is a static analysis tool that examines C/C++ source code without
#   running it, finding bugs, style issues, and potential security problems.
#   
# Why use it:
#   - Catches bugs early (before runtime)
#   - Improves code quality and readability  
#   - Finds dead/unused code that can be removed
#   - Suggests performance optimizations
#   - Ensures consistent coding style
#   - Industry standard tool for professional development
#
# Analysis categories enabled:
#   --enable=warning      : Detects potential bugs (array bounds, uninitialized vars)
#   --enable=style        : Coding style improvements (variable scope, redundant code)
#   --enable=performance  : Performance optimizations (inefficient loops, string ops)
#   --enable=portability  : Cross-platform compatibility issues
#   --enable=unusedFunction: Finds functions that are defined but never called
#
# Example issues it catches:
#   - int arr[5]; arr[10] = 42;           // Array bounds violation
#   - int x; printf("%d", x);             // Uninitialized variable  
#   - for(int i=0; i<strlen(s); i++)      // strlen() in loop (inefficient)
#   - char *ptr; if(ptr = malloc(100))    // Assignment instead of comparison
#   - Static functions never called       // Dead code detection
# -----------------------------------------------------------------------------
cppcheck: re
	@echo "${ORANGE}🔍 Running static code analysis with cppcheck...${RESET}"
#	cppcheck --enable=all --inconclusive --std=c99 --force $(SRC) 2>&1 
	cppcheck --enable=warning,style,performance,portability --enable=unusedFunction $(SRC)
	@echo "${GREEN}✅ Static analysis complete! Check output above for suggestions.${RESET}"

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

.PHONY: all clean fclean re valgrind cppcheck success_message art

.SECONDARY: $(OBJ)

.SILENT:
