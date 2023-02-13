#include <X11/Xlib.h>
#include "png.h"
#include "png_structs.h"

png_img image;
XImage* ximage;

/// @brief Draws the XImage to the screen
void draw(){
    draw_png_xwindow(ximage, image.width, image.height);
}

/// @brief Frees the image pixel data
void exit(){
    for(int y = 0; y<image.height; y++){
        free(image.pixels[y]);
    }
    free(image.pixels);
    //XDestroyImage(ximage);
}

/// @brief Try to load the png 
/// @param path 
/// @return 
int init(char* path){

    // Load the image and connect to the xserver
    if(load_png(path, &image)){
        initialize_xwindow();
        get_ximage_from_png(image, &ximage);
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]){
    // check arguments
    if(argc != 2){
        printf("Expected file argument, closing...\n");
        return 2;
    }
    // Get the path from argv
    char* path = argv[1];
    if(init(path)){
        run_window_loop(&draw, &exit);
    }

    // In
    return 0;
}