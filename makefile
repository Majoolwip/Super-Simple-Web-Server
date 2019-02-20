CC=gcc
SRC=src/main.c src/server.c
TEST_SRC=test/main.c
HEADERS=headers/
NAME=SSWS
optimize=3
.PHONY: debug
debug:
	$(CC) $(SRC) -o $(NAME) -I $(HEADERS)
.PHONY: debug
production:
	$(CC) $(SRC) -O$(optimize) -o $(NAME) -I $(HEADERS)
.PHONY: test
test:
	$(CC) $(TEST_SRC) -o $(NAME)Test -I $(HEADERS)
