# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lpradene <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/08 12:33:43 by lpradene          #+#    #+#              #
#    Updated: 2022/11/08 13:28:55 by lpradene         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Source files
SRCS = ft_isalpha.c \
       ft_isdigit.c \
       ft_isalnum.c \
       ft_isascii.c \
       ft_isprint.c \
       ft_isspace.c \
       ft_strlen.c \
       ft_memset.c \
       ft_bzero.c \
       ft_memcpy.c \
       ft_memmove.c \
       ft_strlcpy.c \
       ft_strlcat.c \
       ft_toupper.c \
       ft_tolower.c \
       ft_strchr.c \
       ft_strrchr.c \
       ft_strncmp.c \
       ft_memchr.c \
       ft_memcmp.c \
       ft_strnstr.c \
       ft_atoi.c \
       ft_calloc.c \
       ft_strdup.c \
       ft_substr.c \
       ft_strjoin.c \
       ft_strcmp.c \
       ft_strtrim.c \
       ft_split.c \
       ft_itoa.c \
       ft_strmapi.c \
       ft_striteri.c \
       ft_putchar_fd.c \
       ft_putstr_fd.c \
       ft_putendl_fd.c \
       ft_putnbr_fd.c \
       ft_lstnew.c \
       ft_lstadd_front.c \
       ft_lstsize.c \
       ft_lstlast.c \
       ft_lstadd_back.c \
       ft_lstdelone.c \
       ft_lstclear.c \
       ft_lstiter.c \
       ft_lstmap.c \
       ft_strtoupper.c \
       ft_strtolower.c

# Paths
SRCS := $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude

# Library name
NAME = libft.a

# Ensure obj directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compilation rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for building the library
$(NAME): $(OBJS)
	ar rc $(NAME) $(OBJS)
	ranlib $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
