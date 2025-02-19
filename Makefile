# Nome do programa
NAME = neotextus

# Compilador e flags
CC = gcc
CFLAGS = -lncurses

# Arquivos de origem
SRC = main.c

# Alvo padrão
all: $(NAME)

# Compilação
$(NAME): $(SRC)
	$(CC) -o $(NAME) $(SRC) $(CFLAGS)

# Limpeza
clean:
	rm -f $(NAME)

install:
	sudo mv $(NAME) /usr/local/bin

.PHONY: all clean
