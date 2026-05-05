CC     = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
SRC    = src/main.c src/lexer.c src/parser.c src/executor.c \
         src/builtins.c src/signals.c src/jobs.c
OBJ    = $(SRC:.c=.o)
TARGET = mysh

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

#use this command if you get " $'\r': command not found" when running the tests, and youre on Linux/WSL
# changes windows line ending \r\n to linux \n
.PHONY: fix_posix
fix_posix:
	find . -name "*.sh" -exec sed -i 's/\r$$//' {} +

# changes linux \n tp windows line ending \r\n
.PHONY: fix_win
fix_win:
	find . -name "*.sh" -exec sed -i 's/$$/\r/' {} +
