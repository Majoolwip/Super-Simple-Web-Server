CC=gcc
SRC=src/main.c src/server.c
HEADERS=headers/
NAME=SSWS
build:
	$(CC) $(SRC) -o $(NAME) -I $(HEADERS)
