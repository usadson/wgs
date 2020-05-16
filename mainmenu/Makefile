CC = clang
CFLAGS = -g -Og -I.
LDFLAGS = -lX11 -lpthread -lxcb -lXau -lXdmcp

test: main.c
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

clean:
	rm -rf test
