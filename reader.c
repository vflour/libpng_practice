#include "png.h"
#include <stdio.h>
typedef unsigned char uchar;

/// @brief Checks the png signature and outputs to consule
/// @param f_ptr 
/// @return 1 if successful
int check_file(FILE* f_ptr){
    // the first 8 bytes of a png are the signature
    uchar sig[8]; 
    fread(sig, 1, 8, f_ptr);

    // if the signature is bad, then close
    if(!png_check_sig(sig, 8)){
        printf("Bad PNG signature, closing...\n");
        return 0;
    }
    return 1;
} 

/// @brief Initialize the two pointer variables for the png. Output to console on failure
/// @param png_ptr 
/// @param info_ptr 
/// @return 1 if successful
int setup_png(png_structp* png_ptr, png_infop* info_ptr){
    // Update the png_ptr pointer to the new struct
    // Meaning you need to dereference the ptr variable and not the ptr itself
    *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
      NULL);

    // If there's no pointer then there's an issue
    if (!png_ptr){
        printf("Out of memory. \n");
        return 0;
    }
  
    // Same applies here with the info struct
    *info_ptr = png_create_info_struct(*png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(png_ptr, NULL, NULL);
        printf("Out of memory. \n");
    }

    return 1;
}

int read_png(FILE* f_ptr){
    // Exit if the file is invalid
    if(!check_file(f_ptr)){
        return 0;
    }
    png_structp png_ptr;
    png_infop info_ptr;
    // Exit if setting up structs failed
    if(!setup_png(&png_ptr, &info_ptr)){
        return 0;
    }

    // dispose structs
    png_destroy_read_struct(png_ptr, info_ptr, NULL);
    return 1;
}


int main(int argc, char *argv[]){
    printf("Starting program \n");
    // check arguments
    if(argc != 2){
        printf("Expecting file argument\n");
    }
    // open the file 
    char* path = argv[1];
    FILE* f_ptr = fopen(path, "rb");

    // pointer exists if file exists
    if(f_ptr){
        if(!read_png(f_ptr)){
            printf("Failed to read png.\n");
        }
        // close at the end
        fclose(f_ptr);
    }else{
        printf("No such file: %s \n", path);
    }

    return 0;
}