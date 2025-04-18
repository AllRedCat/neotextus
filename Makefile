# Nome do programa
NAME = neotextus

# Compilador e flags
CC = gcc
CFLAGS = -lncursesw

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
	rm -f *.txt

install:
	sudo mv $(NAME) /usr/local/bin

.PHONY: all clean
