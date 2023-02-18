// Main program file

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include "png.h"
#include "png_xwindow.h"
#include "png_reader.h"

XImage* ximage;
XImage* saved_image;

/// @brief Draws the XImage to the screen
void draw(){
    draw_png_xwindow(ximage, ximage->width, ximage->height);
}

/// @brief Takes a screenshot and saves it to test_save.png
void save_image(){
    char* path = "test_save.png";

    // Remove the old one
    if(saved_image != NULL){
        XDestroyImage(saved_image);
    }
    get_ximage_from_window(&saved_image);
    // Create a png_image from the ximage
    png_img image = get_png_from_ximage(saved_image);
    save_png(path, &image);
    free_img(image);
    printf("Screenshot written to %s \n", path);
}

/// @brief Frees the image pixel data
void exit_program(){
    printf("Exiting...\n");
    if(saved_image != NULL){
        XDestroyImage(saved_image);
    }
    //XDestroyImage(ximage);
}

/// @brief Try to load the png 
/// @param path 
/// @return 
int init_program(char* path){
    png_img image; 
    // Load the image and connect to the xserver
    if(load_png(path, &image)){
        initialize_xwindow();
        get_ximage_from_png(image, &ximage);
        if(!ximage){
            return ERROR;
        }
        free_img(image);
        return SUCCESS;
    }
    return ERROR;
}

int main(int argc, char* argv[]){
    // check arguments
    if(argc != 2){
        printf("Expected file argument, closing...\n");
        return 2;
    }
    // Get the path from argv
    char* path = argv[1];
    if(init_program(path)){
        run_window_loop(&draw, &exit_program, &save_image);
    }

    return 0;
}