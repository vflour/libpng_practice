#include "png_structs.h"
#include <stdlib.h>

/// @brief Frees the pixel buffer inside the image
/// @param image
void free_img(png_img image){
    for(int y = 0; y<image.height; y++){
        free(image.pixels[y]);
    }
    free(image.pixels);
}