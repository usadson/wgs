CC = clang
INCLUDE = -I. -I/usr/local/include
OPTIMIZATION = -g -Og -c
WARNINGS = -Wall -Wextra -Werror
CFLAGS = $(WARNINGS) $(OPTIMIZATION) $(INCLUDE)

libcg: libcg.c libcg.h
	$(CC) $(CFLAGS) -o $@ libcg.c $(LDFLAGS)

clean:
	rm -rf libcg
