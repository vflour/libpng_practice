#ifndef PNG_STRUCT_HEADER
#define PNG_STRUCT_HEADER

#define ERROR 0
#define SUCCESS 1

typedef unsigned char uchar;

typedef struct PNG_PIX{
    uchar r;
    uchar g;
    uchar b;
    uchar a;
} png_pix;

typedef struct PNG_IMG{
    int width;
    int height;
    png_pix** pixels;
} png_img;

void free_img(png_img image);

#endif