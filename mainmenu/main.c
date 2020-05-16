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

int main(void) {
	Display *display;
	Window window;
	Screen *screen;
	int screenId;
	XEvent ev;

	display = XOpenDisplay(NULL);
	if (display == NULL) {
		fputs("Could not open display", stderr);
		return 1;
	}
	screen = DefaultScreenOfDisplay(display);
	screenId = DefaultScreen(display);

	window = XCreateSimpleWindow(display, RootWindowOfScreen(screen), 0, 0,
								 320, 200, 1, BlackPixel(display, screenId),
								 WhitePixel(display, screenId));

	XSelectInput(display, window, KeyPressMask | KeyReleaseMask
								| KeymapStateMask);

	XClearWindow(display, window);
	XMapRaised(display, window);

	char str[25] = {0}; 
	KeySym keysym = 0;
	int len = 0;
	bool running = true;

	while (running) {
		XNextEvent(display, &ev);
		switch(ev.type) {
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
