// Project - Code Optimization
// System Programming, DGIST, Prof. Yeseong Kim
// 
// You will not turn in this file, so do not need to modify it.
// Read the provided instruction carefully.

#ifndef _BMP_LIB_H_
#define _BMP_LIB_H_

typedef struct 
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct
{
    unsigned int filesize;
    unsigned char* buffer;

    unsigned int width;
    unsigned int height;

    unsigned int pixel_offset;

    Pixel* pixels;
} BMP;


BMP* load_bmp(const char* filename);
void fill_bmp(BMP* bmp, Pixel* pixel_buffer);
void save_bmp(BMP* bmp, const char* filename);
void unload_bmp(BMP* bmp);

#endif
