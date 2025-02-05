# Directories
SRC_DIR = src
OBJ_DIR = obj
LIBFT_DIR = libft
INC_DIR = include

# Source files
SRCS = main.c ls.c
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -I$(LIBFT_DIR)/include

# Library
LIBFT = libft.a

# Executable name
NAME = ls

# Ensure obj directory exists
$(OBJ_DIR):
	mkdir -p $@

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the program
$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LIBFT_DIR)/$(LIBFT) -o $(NAME)

# Build Libft if not compiled
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR) all

all: $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(LIBFT)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
