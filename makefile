CC = gcc

all: filesystem

filesys: filesys.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f filesys
