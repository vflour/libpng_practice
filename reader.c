#include "png.h"
#include <stdio.h>
typedef unsigned char uchar;

int check_file(FILE* f_ptr){
    uchar sig[8]; 
    fread(sig, 1, 8, f_ptr);

    if(!png_check_sig(sig, 8)){
        printf("Bad PNG signature, closing...\n");
        return 0;
    }
    return 1;
} 


int main(int argc, char *argv[]){
    printf("Starting program \n");
    // check arguments
    if(argc != 2){
        printf("Expecting file argument\n");
    }
    // open read
    char* path = argv[1];
    FILE* f_ptr = fopen(path, "rb");

    if(f_ptr){
        if(check_file(f_ptr)){
            printf("Passed check.\n");
        }
        fclose(f_ptr);
    }else{
        printf("No such file: %s \n", path);
    }

    return 0;
}