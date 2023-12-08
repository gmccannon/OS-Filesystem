CC = gcc

all: filesys

filesys: filesys.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f filesys
