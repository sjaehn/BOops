#include "screen.h"
#include <X11/Xlib.h>

int getScreenWidth ()
{
	Display* display = XOpenDisplay(NULL);
	Screen*  screen = DefaultScreenOfDisplay(display);
	return  screen->width;
}

int getScreenHeight ()
{
	Display* display = XOpenDisplay(NULL);
	Screen*  screen = DefaultScreenOfDisplay(display);
	return screen->height;
}
