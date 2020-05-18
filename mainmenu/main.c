/**
 * BSD-2-Clause
 *
 * Copyright (c) 2020 Tristan
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND  ANY  EXPRESS  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED  WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE  DISCLAIMED.  IN  NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE   FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO,  PROCUREMENT  OF
 * SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT,  STRICT  LIABILITY,  OR  TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING  IN  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/stat.h>

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <GL/glew.h>
#include <GL/glx.h>

struct ShaderData {
	GLuint		fragmentShader;
	GLuint		program;
	GLuint		vertexShader;
};

/** Function Prototypes **/
void
cleanShaders();

char *
loadFile(const char *);

bool
loadShader(const char *, GLenum, GLuint *);

bool
loadShaders(void);

void
renderFrame(void);


/** Strings **/
const char *fragShaderPath = "res/fragment_shader.glsl";
const char *vertShaderPath = "res/vertex_shader.glsl";

/** Global variables **/
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
struct ShaderData	mainShader;

int
main(void) {
	/* X info */
	Display *display;
	Window rootWindow;
	Window window;
	Screen *screen;
	int screenId;
	XEvent ev;
	XVisualInfo *visualInfo;
	XSetWindowAttributes windowAttributes;

	/* GLX info */
	Colormap colormap;
	GLXContext context;

	display = XOpenDisplay(NULL);
	if (display == NULL) {
		fputs("Could not open display", stderr);
		return 1;
	}

	screen = DefaultScreenOfDisplay(display);
	screenId = DefaultScreen(display);
	rootWindow = RootWindowOfScreen(screen);

	visualInfo = glXChooseVisual(display, 0, att);

	if (visualInfo == NULL) {
		XCloseDisplay(display);
		printf("\n\tno appropriate visual found\n\n");
		return EXIT_FAILURE;
	}

	colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);

	windowAttributes.colormap = colormap;
	windowAttributes.event_mask = ExposureMask
								| KeyPressMask
								| KeyReleaseMask
								| KeymapStateMask;

	window = XCreateWindow(display, rootWindow, 0, 0,
						   screen->width, screen->height, 0,
						   visualInfo->depth, InputOutput, visualInfo->visual,
						   CWColormap | CWEventMask, &windowAttributes);

	XClearWindow(display, window);
	XMapRaised(display, window);

	char str[25] = {0}; 
	KeySym keysym = 0;
	int len = 0;
	bool running = true;

	context = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(display, window, context);

	if (glewInit() != GLEW_OK) {
		glXDestroyContext(display, context);
		XDestroyWindow(display, window);
		XCloseDisplay(display);
		fputs("Failed to initialize GLEW!\n", stderr);
		return EXIT_FAILURE;
	}

	if (!loadShaders()) {
		glXDestroyContext(display, context);
		XDestroyWindow(display, window);
		XCloseDisplay(display);
		fputs("Failed to load shaders!\n", stderr);
		return EXIT_FAILURE;
	}

	while (running) {
		XNextEvent(display, &ev);

		puts("new event");
		renderFrame();
		glXSwapBuffers(display, window);

		switch(ev.type) {
			case Expose:
				renderFrame();
				glXSwapBuffers(display, window);
				break;
			case KeymapNotify:
				XRefreshKeyboardMapping(&ev.xmapping);
				break;
			case KeyPress:
				len = XLookupString(&ev.xkey, str, 25, &keysym, NULL);
				str[len] = '\0';
				if (len > 0) {
					printf("Key pressed: '%s' %i %zu\n", str, len,
						   (size_t) keysym);
				}
				if (keysym == XK_Escape) {
					running = false;
				}
				break;
			case KeyRelease:
				len = XLookupString(&ev.xkey, str, 25, &keysym, NULL);
				str[len] = '\0';
				if (len > 0) {
					printf("Key released: '%s' %i %zu\n", str, len,
						   (size_t) keysym);
				}
				break;
		}
	}

	/* Cleanup */
	cleanShaders();

	XDestroyWindow(display, window);
	XCloseDisplay(display);

	return EXIT_SUCCESS;
}

void
renderFrame(void) {
	puts("RenderFrame called");
	glClearColor((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

bool
loadShaders(void) {
	mainShader.program = glCreateProgram();
	if (mainShader.program == 0) {
		fputs("[loadShaders] Failed to create shader program!", stderr);
		return false;
	}

	if (!loadShader(vertShaderPath, GL_VERTEX_SHADER,
					&mainShader.vertexShader)) {
		glDeleteProgram(mainShader.program);
		fputs("[loadShaders] Failed to load vertex shader!", stderr);
		return false;
	}

	if (!loadShader(fragShaderPath, GL_FRAGMENT_SHADER,
					&mainShader.fragmentShader)) {
		glDeleteShader(mainShader.vertexShader);
		glDeleteProgram(mainShader.program);
		fputs("[loadShaders] Failed to load fragment shader!", stderr);
		return false;
	}

	glAttachShader(mainShader.program, mainShader.vertexShader);
	glAttachShader(mainShader.program, mainShader.fragmentShader);

	glBindAttribLocation(mainShader.program, 0, "position");

	glLinkProgram(mainShader.program);

	/* Perform checks with glValidateProgram */

	/* bind uniform locations */

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("error: %u\n", (uint32_t) err);

	return true;
}

char *
loadFile(const char *path) {
	char		*buf;
	int			 fd;
	size_t		 len;
	char		*pos;
	ssize_t		 ret;
	struct stat	 status;

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror("open() failure");
		fprintf(stderr, "Failed to open '%s'. "
				"See the error described above.\n", path);
		return NULL;
	}

	if (fstat(fd, &status) == -1) {
		perror("fstat() failure");
		close(fd);
		fprintf(stderr, "Failed to stat() '%s'. "
			   "See the error described above.\n", path);
		return NULL;
	}

	if (!S_ISREG(status.st_mode)) {
		close(fd);
		printf("'%s' isn't a normal file!\n", path);
		return NULL;
	}

	len = (size_t) status.st_size;
	buf = malloc(status.st_size + 1);
	buf[len] = '\0';

	if (buf == NULL) {
		perror("Failed to allocate data for file");
		close(fd);
		fprintf(stderr, "Failed to allocate data for file '%s'!\n", path);
		return NULL;
	}

	pos = buf;
	while (len > 0) {
		ret = read(fd, pos, len);

		if (ret == 0)
			break;

		if (ret == -1) {
			perror("Failed to read from file");
			close(fd);
			fprintf(stderr, "Failed to read from file '%s'!\n", path);
			return NULL;
		}

		len -= ret;
		pos += ret;
	}

	close(fd);
	return buf;
}

bool
loadShader(const char *path, GLenum type, GLuint *dest) {
	GLuint	 shader;
	char	*shaderData;
	GLint	 status;

	shader = glCreateShader(type);
	if (shader == 0) {
		fputs("[loadShader] Failed to glCreateShader()!\n", stderr);
		return false;
	}

	shaderData = loadFile(path);
	if (shaderData == NULL) {
		glDeleteShader(shader);
		fputs("[loadShader] Failed to load shader file!\n", stderr);
		return false;
	}

	glShaderSource(shader, 1, (const char *const *) &shaderData, NULL);
	free(shaderData);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glDeleteShader(shader);
		fputs("[loadShader] Failed to compile shader!\n", stderr);
		return false;
	}

	*dest = shader;
	return true;
}

void
cleanShaders(void) {
	glDetachShader(mainShader.program, mainShader.vertexShader);
	glDetachShader(mainShader.program, mainShader.fragmentShader);
	glDeleteShader(mainShader.vertexShader);
	glDeleteShader(mainShader.fragmentShader);
	glDeleteProgram(mainShader.program);
}
