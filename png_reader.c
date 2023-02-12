#include "png.h"
#include <stdio.h>
typedef unsigned char uchar;
#define PIXEL_SIZE 8

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

/// @brief Custom callback function for handling unknown chunks. Just prints a message
/// @param png_ptr 
/// @param chunk 
/// @return 
int chunk_callback(png_structp png_ptr, png_unknown_chunkp chunk){
    printf("Encountered unknown chunk\n");
    /**
     * If you call the png_set_read_user_chunk_fn() function, then all unknown 
     * chunks which the callback does not handle will be saved when read. 
     * You can cause them to be discarded by returning '1' ("handled") instead of '0'.
     */
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

/// @brief Read the background image and init the r,g,b values
/// @param red 
/// @param green 
/// @param blue 
/// @param png_ptr 
/// @param info_ptr 
/// @return 
int read_bg(uchar* red,uchar* green,uchar* blue, png_structp png_ptr, png_infop info_ptr){
    if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD))
        return 1;
        
    // Get pointer to background
    png_color_16p pBackground;
    png_get_bKGD(png_ptr, info_ptr, &pBackground);

}

/// @brief Reads from the PNG struct
/// @param png_ptr 
/// @param info_ptr 
/// @param f_ptr 
/// @return 
int read_png(png_structp png_ptr, png_infop info_ptr, FILE* f_ptr, int* png_width, int* png_height, png_bytepp* png_data){
    // According to the guide, it takes the file pointer and stores it in the png pointer
    png_init_io(png_ptr, f_ptr);
    // Lets the library know that the first 8 bytes were checked, so it wont find it at the file pointer location
    png_set_sig_bytes(png_ptr, 8);

    // Begin processing the png file
    #if PNG_LIBPNG_VER >= 10504
        png_set_alpha_mode(png_ptr, PNG_ALPHA_STANDARD, PNG_DEFAULT_sRGB);
    #endif

    // Using the high level interface for reading pngs
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_SCALE_16 | PNG_TRANSFORM_PACKING, NULL);
    
    // It's good to get the image height and width
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);

    printf("The height and width are %d and %d \n", height, width);

    // And validate it early before reading
    if (height > PNG_UINT_32_MAX/(sizeof (png_byte)))
        png_error (png_ptr, "Image is too tall to process in memory\n");

    if (width > PNG_UINT_32_MAX/PIXEL_SIZE )
        png_error (png_ptr, "Image is too wide to process in memory\n");

    // Get the row pointers 
    *png_data = png_get_rows(png_ptr, info_ptr);
    *png_width = width;
    *png_height = height;
    
    return 0;
}

int open_png(FILE* f_ptr, int* width, int* height, png_bytepp* png_data){
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

    read_png(png_ptr, info_ptr, f_ptr, width, height, png_data);
    // dispose structs
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 1;
}


int load_png(char* path, int* width, int* height, png_bytepp* png_data){
    FILE* f_ptr = fopen(path, "rb");

    // pointer exists if file exists
    if(f_ptr){
        if(!open_png(f_ptr, width, height, png_data)){
            printf("Failed to read png.\n");
        }
        // close at the end
        fclose(f_ptr);
        return 0;
    }else{
        printf("No such file: %s \n", path);
        return 0;
    }

    return 1;
}