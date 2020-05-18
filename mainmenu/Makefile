CC = clang
INCLUDE = -I. -I/usr/local/include -I../libcoregraphics
LIBRARIES = -lpthread -lX11 -lGLEW -lGLU -lm -lGL ../libcoregraphics/libcg
OPTIMIZATION = -g -Og
WARNINGS = -Wall -Wextra -Werror
CFLAGS = $(WARNINGS) $(OPTIMIZATION) $(INCLUDE)
LDFLAGS = $(LIBRARIES)

test: main.c ../libcoregraphics/libcg
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

clean:
	rm -rf test
