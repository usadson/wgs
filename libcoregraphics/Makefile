CC = clang
INCLUDE = -I. -I/usr/local/include
OPTIMIZATION = -g -Og -c
WARNINGS = -Wall -Wextra -Werror
CFLAGS = $(WARNINGS) $(OPTIMIZATION) $(INCLUDE)

libcg: stb_image libcg.c libcg.h
	$(CC) $(CFLAGS) -o $@ libcg.c $(LDFLAGS)

stb_image:
	$(CC) -c -O3 -o $@ stb_image.c

clean:
	rm -rf libcg
