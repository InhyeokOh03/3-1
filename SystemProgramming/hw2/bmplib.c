// Assignment 2 - Efficient C Programming
// System Programming, DGIST, Prof. Yeseong Kim
// 
// You will not turn in this file, so do not need to modify it.
// Read the provided instruction carefully.
//
// - Very minimal library to read/write a BMP file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmplib.h"

BMP* load_bmp(const char* filename) {
    // Open file
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Create a BMP data structure
    BMP* bmp = (BMP*)malloc(sizeof(BMP));

    // Get the file size
    fseek(fp, 0, SEEK_END);
    bmp->filesize = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET); // seek back to beginning of file

    // Create buffer and read the data
    bmp->buffer = (unsigned char*)malloc(bmp->filesize);
    unsigned int res = fread(bmp->buffer, 1, bmp->filesize, fp);
    if (res != bmp->filesize) {
        fprintf(stderr, "Cannot read the file:\t%s\n", filename);
        exit(1);
    }
    fclose(fp);

    if (bmp->filesize < 28 || bmp->buffer[0] != 'B' || bmp->buffer[1] != 'M') {
        fprintf(stderr, "This file is not a BMP file:\t%s\n", filename);
        exit(1);
    }

    // Parse metadata information
    unsigned short depth;
    memcpy(&bmp->pixel_offset, bmp->buffer + 10, 4);
    memcpy(&bmp->width, bmp->buffer + 18, sizeof(bmp->width));
    memcpy(&bmp->height, bmp->buffer + 22, sizeof(bmp->height));
    memcpy(&depth, bmp->buffer + 28, sizeof(depth));

    if (depth != 24) {
        fprintf(stderr, "Unsupported BMP depth:\t%s\n", filename);
        exit(1);
    }

    if (bmp->width % 32 != 0) {
        // Note: In BMP, the pixel storage are packed in rows.
        // (rounded up to a multiple of 4 bytes by padding)
        // For simplicity, we only handle the image which does not have padding
        fprintf(stderr, "Unsupported BMP width:\t%s\n", filename);
        exit(1);
    }


    bmp->pixels = (Pixel*)(bmp->buffer + bmp->pixel_offset);

    return bmp;
}

void fill_bmp(BMP* bmp, Pixel* pixel_buffer) {
    memcpy(bmp->pixels, pixel_buffer,
            bmp->width * bmp->height * sizeof(Pixel));
}

void save_bmp(BMP* bmp, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    fwrite(bmp->buffer, 1, bmp->filesize, fp);
    fclose(fp);
}


void unload_bmp(BMP* bmp) {
    free(bmp);
}


