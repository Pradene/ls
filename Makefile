# Directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc

LIBFT_DIR = libft
LIBFT = libft.a

# Source files
SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Compiler settings
CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -I$(LIBFT_DIR)/inc -g

# Executable name
NAME = ft_ls

all: $(NAME)
	@echo "\033[1;32m[OK]\033[0m Build complete: $(NAME)"

# Build Libft if not compiled
$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR) all

# Compile the program
$(NAME): $(LIBFT) $(OBJS)
	@$(CC) $(OBJS) $(LIBFT_DIR)/$(LIBFT) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "\033[1;36m[CC]\033[0m $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "\033[1;31m[CLEAN]\033[0m Removing object files"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "\033[1;31m[FCLEAN]\033[0m Removing $(NAME)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re