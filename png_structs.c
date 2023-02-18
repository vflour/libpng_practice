#include "png_structs.h"

void free_img(png_img image){
    for(int y = 0; y<image.height; y++){
        free(image.pixels[y]);
    }
    free(image.pixels);
}