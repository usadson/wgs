CC = clang
CFLAGS = -g -Og -I. -I/usr/local/include
LDFLAGS = -L/usr/local/lib -L/usr/lib64 \
	  -lpthread \
	  -lX11 \
	  -lGLEW -lGLU -lm -lGL

test: main.c
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

clean:
	rm -rf test
