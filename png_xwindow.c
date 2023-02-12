/// This initializes the x11 window so that there may be something to draw on.
/// It also handles any routines for x11 to use
/// Credit to this guide: http://mech.math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html
#include <X11/Xlib.h>

static int WIDTH = 500;
static int HEIGHT = 500;
// You only want it to check for exposure for now
static long ALLOWED_INPUTS = ExposureMask;

// Kept as globals since there's only one window
Display *display;
int screen;
Window window;
GC gc;
unsigned long black, white;

/// @brief Based off the guide's initialization of the x window. 
void initialize_xwindow(){
    // Create an X connection and initialize the screen var
    display = XOpenDisplay((char *)0);
    screen = DefaultScreen(display);
    black = BlackPixel(display, screen);
    white = WhitePixel(display, screen);

    // You can then create the window
    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, WIDTH, HEIGHT, 5, white, black);
    // Window property setting
    XSetStandardProperties(display, window, "Png Viewer", "Img", None, NULL, 0, NULL);
    // Initialize what inputs are allowed. 
	XSelectInput(display, window, ALLOWED_INPUTS);

    // Create the graphics context
    gc = XCreateGC(display, window, 0,0);    

    // And now set the background and foreground
    XSetBackground(display, gc, white);
	XSetForeground(display, gc, black);
}

void close_xwindow(){

}

void draw_png_xwindow(){

}