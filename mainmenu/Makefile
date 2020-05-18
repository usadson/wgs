CC = clang
INCLUDE = -I. -I/usr/local/include -I../libcoregraphics
LIBCG = ../libcoregraphics/libcg ../libcoregraphics/stb_image
LIBRARIES = -L/usr/local/lib -L/usr/lib64 -lpthread -lX11 -lGLEW -lGLU -lm -lGL $(LIBCG)
OPTIMIZATION = -g -Og
WARNINGS = -Wall -Wextra -Werror
CFLAGS = $(WARNINGS) $(OPTIMIZATION) $(INCLUDE)
LDFLAGS = $(LIBRARIES)

test: main.c ../libcoregraphics/libcg
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

clean:
	rm -rf test
