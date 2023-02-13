typedef struct PNG_PIX{
    char r;
    char g;
    char b;
    char a;
} png_pix;

typedef struct PNG_IMG{
    int width;
    int height;
    png_pix** pixels;
} png_img;