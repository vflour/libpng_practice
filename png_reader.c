/// The reader class file
/// There's a bunch of unused functions. Don't worry about it lol

#include "png.h"
#include <stdio.h>
#include <X11/Xlib.h>
#include "png_structs.h"

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


/// @brief Reads from the PNG struct
/// @param png_ptr 
/// @param info_ptr
/// @param image
/// @return 
int read_png(png_structp png_ptr, png_infop info_ptr, png_img* image){
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
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    // Allocate the pixels in the struct
    image->width = width;
    image->height= height;
    image->pixels = malloc(sizeof(png_pix*)*height);

    // Get pixels
    for(int y = 0; y < height; y++){
        png_bytep row = row_pointers[y];
        image->pixels[y] = malloc(sizeof(png_pix)*width);
        
        // Populate each row with the actual pixel data
        for( int x = 0; x < width; x++){
            png_bytep pixel = &(row[x * 4]);
            image->pixels[y][x].r = pixel[0];
            image->pixels[y][x].g = pixel[1];
            image->pixels[y][x].b = pixel[2];
            image->pixels[y][x].a = pixel[3];
        }
    }

    
    return 0;
}

int open_png_read(FILE* f_ptr, png_img* image){
    // Exit if the file is invalid
    if(!check_file(f_ptr)){
        return 0;
    }
    // Update the png_ptr pointer to the new struct
    // Meaning you need to dereference the ptr variable and not the ptr itself
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
      NULL);

    // If there's no pointer then there's an issue
    if (!png_ptr){
        perror("Could not initialize PNG write struct \n");
        return 0;
    }
  
    // Same applies here with the info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(png_ptr, NULL, NULL);
        perror("Could not initialize PNG info struct \n");
        return 0;
    }

    // use setjmp since libpng relies on it for errors
    if (setjmp(png_jmpbuf(png_ptr))){
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        perror("Encountered an error while reading the png\n");
        return 0;
    }
    // According to the guide, it takes the file pointer and stores it in the png pointer
    png_init_io(png_ptr, f_ptr);
    // Now you can read
    read_png(png_ptr, info_ptr, image);

    // dispose structs
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 1;
}

int write_png(png_structp png_ptr, png_infop info_ptr, png_img* image){
    int width = image->width;
    int height = image->height;
    // Write to the IHDR block
    png_set_IHDR(png_ptr, info_ptr, width, height,
        8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Initialize the buffer for storing a row
    png_bytepp row_pointers = png_malloc(png_ptr, height*(sizeof (png_bytep)));
    for (int y=0; y<height; y++){
        // I think you allocate first
        row_pointers[y] = png_malloc(png_ptr, width*sizeof(png_bytep)*4);
        // Then write to the rows
        png_pix* img_row = image->pixels[y];
        png_bytep wr_row = row_pointers[y];

        for(int x=0; x<width; x++){
            png_pix pixel = img_row[x];
            png_bytep pix_address = &(wr_row[x*4]);
            pix_address[0] = pixel.r;
            pix_address[1] = pixel.g;
            pix_address[2] = pixel.b;
            pix_address[3] = pixel.a;
        }
    }

    // Set rows
    png_set_rows(png_ptr, info_ptr, row_pointers);

    // Write the png to the file
    // No transforms
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
}

int open_png_write(FILE* f_ptr, png_img* image){
    // Set up png_ptr for write
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr){
        perror("Could not initialize PNG write struct \n");
        return 0;
    }

    // Setup info ptr
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        png_destroy_write_struct(png_ptr, NULL);
        perror("Could not initialize PNG info struct \n");
        return 0;
    }

    //Setup error handling
    if(setjmp(png_jmpbuf(png_ptr))){
        png_destroy_write_struct(&png_ptr, &info_ptr);
        perror("Encountered an error while writing the png\n");
        return 0;
    }

    // Initialize I/O
    png_init_io(png_ptr, f_ptr);
    // Commence writing
    write_png(png_ptr, info_ptr, image);
        
    return 1;
}


int load_png(char* path, png_img* image){
    FILE* f_ptr = fopen(path, "rb");

    // pointer exists if file exists
    if(f_ptr){
        if(!open_png_read(f_ptr, image)){
            printf("Failed to read png.\n");
            fclose(f_ptr);
            abort();
        }
        // close at the end
        fclose(f_ptr);
        return 1;
    }

    printf("No such file: %s \n", path);
    return 0;
}

int save_png(char* path, png_img* image){
    FILE* f_ptr = fopen(path, "wb");

    if(f_ptr){
        if(!open_png_write(f_ptr, image)){
            printf("Failed to write png.\n");
            fclose(f_ptr);
            abort();
        }
        fclose(f_ptr);
        return 1;
    }

    printf("Could not open stream for writing to %s. \n", path);
    return 0;
}