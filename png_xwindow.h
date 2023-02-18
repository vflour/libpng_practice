#include "png_structs.h"
#include <X11/Xlib.h>

void draw_png_xwindow(XImage* image, int width, int height);
void get_ximage_from_window(XImage** ximage);
int initialize_xwindow();
void get_ximage_from_png(png_img image, XImage** ximage);
void run_window_loop( void(*draw_callback)(), void (*exit_callback)(), void (*save_callback)() );
png_img get_png_from_ximage(XImage* ximage);
