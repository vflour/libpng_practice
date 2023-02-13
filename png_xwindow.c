/// This initializes the x11 window so that there may be something to draw on.
/// It also handles any routines for x11 to use
/// Credit to this guide: http://mech.math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html
#include <X11/Xlib.h>
#include "png.h"
#include "png_structs.h"

static int WIDTH = 500;
static int HEIGHT = 500;
static long ALLOWED_INPUTS = ExposureMask | ClientMessage | KeyPressMask;

int scroll_x = 0;
int scroll_y = 0;

// Kept as globals since there's only one window
Display *display;
int screen;
Window window;
GC gc;
Visual *visual;
int depth;
unsigned long black, white;

unsigned long get_rgb_pixel(uchar r, uchar g, uchar b){
    return b + (g << 8) + (r << 16);
}

void get_ximage_from_png(png_img image, XImage** ximage){
    int format = ZPixmap;
    unsigned int width = image.width;
    unsigned int height = image.height;
    char* image_data = malloc(width * height * sizeof(unsigned long));

    *ximage = XCreateImage(display, visual, depth, ZPixmap, 0, image_data, width, height, 16, 0);
    int status = XInitImage(*ximage);
    if(status == 0){
        printf("Could not init ximage idk\n");
        return 1;
    }

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            png_pix pixel = image.pixels[y][x];
            unsigned long pixel_color = get_rgb_pixel(pixel.r, pixel.g, pixel.b);
            XPutPixel(*ximage, x, y, pixel_color);
        }
    }
}

/// @brief Based off the guide's initialization of the x window. 
int initialize_xwindow(){
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
    // Get the visual
    XWindowAttributes attr;
    XGetWindowAttributes(display, window, &attr);

    visual = attr.visual;
    depth = DefaultDepth(display, screen);

    printf("Depth: %d \n", depth);

    // And now set the background and foreground
    XSetBackground(display, gc, white);
	XSetForeground(display, gc, black);

    // Obtain an atom name for deleting the window
    // So that the client may receive a message that the window is closing
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(display, window, &wmDelete, 1);

    // Clear the window and bring it on top
	XClearWindow(display, window);
	XMapRaised(display, window);
}

/// @brief Frees resources taken by xwindow
void close_xwindow(){
	XFreeGC(display, gc);
	XDestroyWindow(display,window);
	XCloseDisplay(display);
}

void run_window_loop( void(*exposure_callback)(), void (*exit_callback)() ){
    XEvent event;
    while(1) {		
		// Read the next event and run the appropriate routine
		XNextEvent(display, &event);

        switch(event.type){
            case Expose: 
                (exposure_callback)();
                break;
            case ClientMessage: 
                close_xwindow();
                (exit_callback)();
                return;
                
            case KeyPress:
                if(event.xkey.keycode == 113){
                    scroll_x += 10;
                }
                if(event.xkey.keycode == 114){
                    scroll_x -= 10;
                }
                if(event.xkey.keycode == 116){
                    scroll_y -= 10;
                }
                if(event.xkey.keycode == 111){
                    scroll_y += 10;
                }
                (exposure_callback)();
        }
	}
}

void draw_png_xwindow(XImage* image, int width, int height){
    XPutImage(display, window, gc, image, 0, 0, scroll_x, scroll_y, width, height);
}