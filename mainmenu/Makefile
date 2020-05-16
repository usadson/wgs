CC = clang
CFLAGS = -g -Og -I. -I/usr/local/include
LDFLAGS = -L/usr/local/lib \
	  -lpthread \
	  -lX11 -lxcb -lXau -lXdmcp

test: main.c
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

clean:
	rm -rf test
