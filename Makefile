NAME = minishell

CC = cc

CFLAG = -Wall -Wextra -Werror 

SRC = 

O_DIR = objdir

OBJ = $(addprefix $(O_DIR)/,$(SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAG) $(OBJ) -o $(NAME)
	@echo "\033[33m** Program created **\033[0m"

$(O_DIR):
	mkdir -p $@

$(O_DIR)/%.o: %.c minishell.h | $(O_DIR)
	$(CC) $(CFLAG) $< $@

clean:
	rm -f $(O_DIR)
	@echo "\033[33m** Object files deleted **\033[0m"

fclean: clean
	rm -f $(NAME)
	@echo "\033[33m** Program deleted **\033[0m"

re: fclean all

.PHONY: all clean fclean re

.SECONDARY: $(OBJ)

.SILENT:
