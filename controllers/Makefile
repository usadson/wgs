CC = clang
CFLAGS = -g -Og
LDFLAGS = -lbluetooth

test: main.c
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

clean:
	rm -rf test
