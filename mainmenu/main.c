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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

/** Function Prototypes **/
void renderFrame(void);

/** Global variables **/
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

int main(void) {
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

	printf("\n\tvisual %p selected\n", (void *) visualInfo->visualid);

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

// 	window = XCreateSimpleWindow(display, RootWindowOfScreen(screen), 0, 0,
// 								 screen->width, screen->height, 0, 0,
// 								 WhitePixel(display, screenId));

	XClearWindow(display, window);
	XMapRaised(display, window);

	char str[25] = {0}; 
	KeySym keysym = 0;
	int len = 0;
	bool running = true;

	context = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(display, window, context);

	while (running) {
		XNextEvent(display, &ev);

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

	XDestroyWindow(display, window);
	XCloseDisplay(display);

	return 1;
}

void renderFrame(void) {
	puts("ok");
	glClearColor(0.33, 0.25, 0.99, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}
