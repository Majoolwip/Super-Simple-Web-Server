CC=gcc
SRC=src/main.c src/server.c libs/Simple-Web-Minimizer/src/web_minimizer.c
TEST_SRC=test/main.c
HEADERS=headers/
MIN_HEADERS=libs/Simple-Web-Minimizer/headers
NAME=SSWS
optimize=3
.PHONY: debug
debug:
	$(CC) $(SRC) -o $(NAME) -I $(HEADERS) -I $(MIN_HEADERS)
.PHONY: debug
production:
	$(CC) $(SRC) -O$(optimize) -o $(NAME) -I $(HEADERS) -I $(MIN_HEADERS)
.PHONY: test
test:
	$(CC) $(TEST_SRC) -o $(NAME)Test -I $(HEADERS) -I $(MIN_HEADERS)
