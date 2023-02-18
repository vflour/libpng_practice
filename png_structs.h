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

/// @brief Frees the pixel buffer inside the image
/// @param image
void free_img(png_img image);