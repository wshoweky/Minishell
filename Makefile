NAME = minishell

CC = cc 

CFLAG = -Wall -Wextra -Werror 

SRC = src/main.c src/parser/tokens.c src/parser/tokens_utils.c #src/shelly.c

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
	@echo "\033[33m** Program created **\033[0m"

$(O_DIR):
	mkdir -p $(O_DIR)/src
	mkdir -p $(O_DIR)/src/parser

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

.PHONY: all clean fclean re

.SECONDARY: $(OBJ)

.SILENT:
