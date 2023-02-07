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

int read_png(png_structp png_ptr, png_infop info_ptr, FILE* f_ptr){
    // According to the guide, it takes the file pointer and stores it in the png pointer
    png_init_io(png_ptr, f_ptr);
    // Lets the library know that the first 8 bytes were checked, so it wont find it at the file pointer location
    png_set_sig_bytes(png_ptr, 8);

    // Begin processing the png file
    /*
     * png_read_info() is the first libpng call we've seen that does any real work. 
     * It reads and processes not only the PNG file's IHDR chunk but also any other chunks up to the first IDAT 
     * (i.e., everything before the image data). For colormapped images this includes the PLTE chunk and possibly 
     * tRNS and bKGD chunks. It typically also includes a gAMA chunk; perhaps cHRM, sRGB, or iCCP; and often tIME 
     * and some tEXt chunks. All this information is stored in the information struct and some in the PNG struct, too, 
     * but for now, all we care about is the contents of IHDR--specifically, the image width and height:
     */
    png_read_info(png_ptr, info_ptr);
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
      &color_type, NULL, NULL, NULL);

    printf("The width is %d and the height is %d\n", width, height);
    
    return 0;
}

int open_png(FILE* f_ptr){
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
    // use setjmp since libpng relies on it for errors
    if (setjmp(png_jmpbuf(png_ptr))){
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        printf("Encountered an error while reading the png\n");
        return 0;
    }

    read_png(png_ptr, info_ptr, f_ptr);
    // dispose structs
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 1;
}


int main(int argc, char *argv[]){
    printf("Starting program \n");
    // check arguments
    if(argc != 2){
        printf("Expected file argument, closing...\n");
        return 2;
    }
    // open the file 
    char* path = argv[1];
    FILE* f_ptr = fopen(path, "rb");

    // pointer exists if file exists
    if(f_ptr){
        if(!open_png(f_ptr)){
            printf("Failed to read png.\n");
        }
        // close at the end
        fclose(f_ptr);
    }else{
        printf("No such file: %s \n", path);
    }

    return 0;
}