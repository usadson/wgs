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

#include "libcg.h"

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

#include "stb_image.h"

/** Function Prototypes **/
void
checkForErrors(const char *, const char *);

char *
loadFile(const char *);

bool
loadShader(const char *, GLenum, GLuint *);

/** Global variables **/
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
bool loopState = true;

/* Xorg info */
Display *display;
Window rootWindow;
Window window;
Screen *screen;
int screenId;
XEvent event;
XVisualInfo *visualInfo;
XSetWindowAttributes windowAttributes;

Colormap colormap;
GLXContext context;

/* User info */
static CGRenderFunc renderFunction = NULL;
static CGShutdownFunc shutdownFunc = NULL;

void
CGCleanError(void) {
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}

bool
CGInitialize(void) {
	display = XOpenDisplay(NULL);

	if (display == NULL) {
		fputs("Could not open display", stderr);
		return false;
	}

	screen = DefaultScreenOfDisplay(display);
	screenId = DefaultScreen(display);
	rootWindow = RootWindowOfScreen(screen);

	visualInfo = glXChooseVisual(display, 0, att);

	if (visualInfo == NULL) {
		XCloseDisplay(display);
		printf("\n\tno appropriate visual found\n\n");
		return false;
	}

	colormap = XCreateColormap(display, rootWindow, visualInfo->visual,
							   AllocNone);

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

	context = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(display, window, context);

	if (glewInit() != GLEW_OK) {
		glXDestroyContext(display, context);
		XDestroyWindow(display, window);
		XCloseDisplay(display);
		fputs("Failed to initialize GLEW!\n", stderr);
		return true;
	}

	return true;
}

int
CGStart(void) {
	char str[25] = { 0 }; 
	KeySym keysym = 0;
	int len = 0;

	while (loopState) {
		while (XCheckMaskEvent(display, -1, &event)) {
			switch(event.type) {
				case KeymapNotify:
					XRefreshKeyboardMapping(&event.xmapping);
					break;
				case KeyPress:
					len = XLookupString(&event.xkey, str, 25, &keysym, NULL);
					str[len] = '\0';
					if (len > 0) {
						printf("Key pressed: '%s' %i %zu\n", str, len,
							(size_t) keysym);
					}
					if (keysym == XK_Escape) {
						CGSetShutdown(CG_SR_DEBUG_ESCAPEKEY);
					}
					break;
				case KeyRelease:
					len = XLookupString(&event.xkey, str, 25, &keysym, NULL);
					str[len] = '\0';
					if (len > 0) {
						printf("Key released: '%s' %i %zu\n", str, len,
							(size_t) keysym);
					}
					break;
				default:
					break;
			}
		}

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		checkForErrors("renderFrame", "preRender");
		if (renderFunction)
			renderFunction(1.0);
		checkForErrors("renderFrame", "postRender");

		glXSwapBuffers(display, window);
	}

	/* Cleanup */
	if (shutdownFunc)
		shutdownFunc();
	CGCleanError();

	return EXIT_SUCCESS;
}

void
CGDeleteShader(struct CGShaderData *shader) {
	glDetachShader(shader->program, shader->vertexShader);
	glDetachShader(shader->program, shader->fragmentShader);

	glDeleteShader(shader->vertexShader);
	glDeleteShader(shader->fragmentShader);

	glDeleteProgram(shader->program);
}

bool
CGLoadShader(struct CGShaderData *shader, struct CGShaderInitData *initInfo) {
	size_t i;

	shader->program = glCreateProgram();
	if (shader->program == 0) {
		fputs("[CGLoadShader] Failed to create shader program!\n", stderr);
		return false;
	}

	if (!loadShader(initInfo->vertexShaderFilePath, GL_VERTEX_SHADER,
					&shader->vertexShader)) {
		glDeleteProgram(shader->program);
		fputs("[CGLoadShader] Failed to load vertex shader!\n", stderr);
		return false;
	}

	if (!loadShader(initInfo->fragmentShaderFilePath, GL_FRAGMENT_SHADER,
					&shader->fragmentShader)) {
		glDeleteShader(shader->vertexShader);
		glDeleteProgram(shader->program);
		fputs("[CGLoadShader] Failed to load fragment shader!\n", stderr);
		return false;
	}

	glAttachShader(shader->program, shader->vertexShader);
	glAttachShader(shader->program, shader->fragmentShader);

	for (i = 0; i < initInfo->attributesCount; i++)
		glBindAttribLocation(shader->program, i, initInfo->attributes[i]);

	glLinkProgram(shader->program);

	/* Perform checks with glValidateProgram */

	/* bind uniform locations */

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

	checkForErrors("loadShader", "preLoad");

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
		char errorLog[4096];
		errorLog[0] = '\0';

		fputs("[loadShader] Failed to compile shader!\n", stderr);
		checkForErrors("loadShader", "compileFailure");

		glGetShaderInfoLog(shader, sizeof(errorLog), NULL, errorLog);
		if (*errorLog == '\0')
			fputs("[loadShader] ShaderLog didn't have anything to report.\n",
				  stderr);
		else
			fprintf(stderr, "[loadShader] ShaderLog: \"%s\"\n", errorLog);

		glDeleteShader(shader);
		checkForErrors("loadShader", "end");

		return false;
	}

	*dest = shader;
	return true;
}

bool
CGLoadMesh(struct CGMeshData *mesh, struct CGMeshInitData *initData) {
	mesh->count = initData->vertexCount;

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, initData->verticesSize, initData->vertices,
				 GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, initData->dimensions, GL_FLOAT, GL_FALSE,
						  initData->dimensions * sizeof(GLfloat), NULL);

	return true;
}

void
checkForErrors(const char *namespace, const char *section) {
	const char *message;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		switch (err) {
			case GL_INVALID_ENUM:
				message = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				message = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				message = "GL_INVALID_OPERATION";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				message = "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				message = "GL_OUT_OF_MEMORY";
				break;
			case GL_STACK_OVERFLOW:
				message = "GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				message = "GL_STACK_UNDERFLOW";
				break;
			default:
				message = "undefined";
				break;
		}
		fprintf(stderr, "[%s] [%s] [OpenGLError] %s\n", namespace, section,
				message);
	}
}

void
CGSetShutdownFunc(CGShutdownFunc func) {
	shutdownFunc = func;
}

void
CGSetRenderFunc(CGRenderFunc func) {
	renderFunction = func;
}

void
CGSetShutdown(enum CGShutdownReason reason) {
	/* reason unused atm */
	((void) reason);

	/* Set CGStart's loopState variable to false, which will break the loop. */
	loopState = false;
}

void
CGDeleteMesh(struct CGMeshData *mesh) {
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteVertexArrays(1, &mesh->vao);
}

bool
CGLoadImage(struct CGImage *image, struct CGImageInitData *initData) {
	int width;
	int height;
	int nrChannels;
	unsigned char *data;

	/* Create OpenGL buffer */
	glGenTextures(1, &image->texture);
	glBindTexture(GL_TEXTURE_2D, image->texture);

	/* Load image using stb_image.
	 * TODO: we can use the best/fastest image loader by checking the type from
	 * initData.type */
	data = stbi_load(initData->path, &width, &height, &nrChannels, 0);

	image->height = height;
	image->width = width;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return true;
}

void
CGDeleteImage(struct CGImage *image) {
	glDeleteTextures(1, &image->texture);
}
