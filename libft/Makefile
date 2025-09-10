NAME = libft.a
BONUS_NAME = .bonus

SRCS = src/ft_isdigit.c src/ft_isalnum.c src/ft_isascii.c src/ft_isprint.c src/ft_strlen.c src/ft_memset.c src/ft_isalpha.c \
	src/ft_bzero.c src/ft_memcpy.c src/ft_memmove.c src/ft_strlcpy.c src/ft_strlcat.c src/ft_toupper.c src/ft_tolower.c \
	src/ft_strchr.c src/ft_strrchr.c src/ft_strncmp.c src/ft_memchr.c src/ft_memcmp.c src/ft_strnstr.c src/ft_atoi.c \
	src/ft_calloc.c src/ft_strdup.c src/ft_substr.c src/ft_strjoin.c src/ft_strtrim.c src/ft_split.c  src/ft_itoa.c \
	src/ft_strmapi.c src/ft_striteri.c src/ft_putchar_fd.c src/ft_putstr_fd.c src/ft_putnbr_fd.c src/ft_putendl_fd.c src/ft_strcmp.c

SRC_BONUS = src/ft_lstnew_bonus.c src/ft_lstadd_front_bonus.c src/ft_lstsize_bonus.c src/ft_lstlast_bonus.c src/ft_lstadd_back_bonus.c src/ft_lstdelone_bonus.c\
	    src/ft_lstclear_bonus.c src/ft_lstiter_bonus.c src/ft_lstmap_bonus.c

SRC_PRINTF = printf/ft_printf.c printf/is_char.c printf/is_str.c printf/is_ptr.c printf/is_signed.c printf/is_unsigned.c

SRC_GNL = get_next_line/get_next_line.c get_next_line/get_next_line_utils.c

SRC_GNL_BONUS = get_next_line/get_next_line_bonus.c get_next_line/get_next_line_utils_bonus.c

OBJDIR = obj

OBJ = $(SRCS:src/%.c=$(OBJDIR)/%.o)

OBJ_BONUS = $(SRC_BONUS:src/%.c=$(OBJDIR)/%.o)

OBJ_PRINTF = $(SRC_PRINTF:printf/%.c=$(OBJDIR)/%.o)

OBJ_GNL = $(SRC_GNL:get_next_line/%.c=$(OBJDIR)/%.o)

OBJ_GNL_BONUS = $(SRC_GNL_BONUS:get_next_line/%.c=$(OBJDIR)/%.o)

FLAGS = -Wall -Wextra -Werror -I.

VPATH = src printf get_next_line

all: $(NAME)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	cc $(FLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(OBJ) $(OBJ_PRINTF) $(OBJ_GNL)
	ar -rcs $(NAME) $(OBJ) $(OBJ_PRINTF) $(OBJ_GNL)

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(OBJ) $(OBJ_BONUS) $(OBJ_PRINTF) $(OBJ_GNL_BONUS)
	ar -rcs $(NAME) $(OBJ) $(OBJ_BONUS) $(OBJ_PRINTF) $(OBJ_GNL_BONUS)
	@touch $(BONUS_NAME)

clean:
	rm -rf $(OBJDIR)
	@rm -f $(BONUS_NAME)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re
