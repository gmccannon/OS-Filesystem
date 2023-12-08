CC = gcc
CFLAGS = -Wall -Wextra

all: filesystem

filesystem: filesys.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f filesystem
