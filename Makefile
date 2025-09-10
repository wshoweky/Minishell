NAME = minishell

CC = cc

CFLAG = -Wall -Wextra -Werror 

SRC = 

O_DIR = objdir

OBJ = $(addprefix $(O_DIR)/,$(SRC:.c=.o))

LIBFT_DIR = ./libft


LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME) $(LIBFT)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(OBJ) $(LIBFT) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAG) -I$(LIBFT_DIR) $< $@

clean:
	rm -f $(OBJ)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

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
