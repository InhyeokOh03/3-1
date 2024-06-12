#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmplib.h"
#include "hw2.h"

static Pixel convolution(Pixel* input, int x, int y, int width, int height, float* filter) {
    double r = 0;
    double g = 0;
    double b = 0;

    for (int dy = -1; dy <= 1; ++dy) {
        int iy = y + dy;
        if (iy < 0 || iy >= height) continue;

        for (int dx = -1; dx <= 1; ++dx) {
            int ix = x + dx;
            if (ix < 0 || ix >= width) continue;

            int filter_idx = (dy + 1) * 3 + (dx + 1);
            int image_idx = iy * width + ix;

            r += input[image_idx].r * filter[filter_idx];
            g += input[image_idx].g * filter[filter_idx];
            b += input[image_idx].b * filter[filter_idx];
        }
    }

    Pixel p;
    p.r = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
    p.g = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
    p.b = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));

    return p;
}

void filter_optimized(void* args[]) {
    unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = args[2];
    Pixel* output = args[3];
    float* filter = args[4];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            output[y * width + x] = convolution(input, x, y, width, height, filter);
        }
    }
}


// // Assignment 2 - Efficient C Programming
// // System Programming, DGIST, Prof. Yeseong Kim
// // 
// // YOU WILL TURN IN THIS FILE.
// // Read the provided instruction carefully.

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #include "bmplib.h"
// #include "hw2.h"

// // This implementation is simply copied from "main.c".
// // Your job is to modify and optimize it for better performance!

// static Pixel convolution(
//         Pixel* input, int x, int y, int width, int height, float* filter) {
//     double r = 0;
//     double g = 0;
//     double b = 0;

//     for (int dx = -1; dx < 2; ++dx) {
//         for (int dy = -1; dy < 2; ++dy) {
//             if ((y + dy) < 0 || (y + dy) >= height)
//                 continue;

//             if ((x + dx) < 0 || (x + dx) >= width)
//                 continue;

//             r += input[(x+dx)+(y+dy)*width].r * filter[(dx+1)+(dy+1)*3];
//             g += input[(x+dx)+(y+dy)*width].g * filter[(dx+1)+(dy+1)*3];
//             b += input[(x+dx)+(y+dy)*width].b * filter[(dx+1)+(dy+1)*3];
//         }
//     }

//     if (r < 0) r = 0;
//     if (g < 0) g = 0;
//     if (b < 0) b = 0;

//     if (r > 255) r = 255;
//     if (g > 255) g = 255;
//     if (b > 255) b = 255;
    
//     Pixel p;
//     memset(&p, 0, sizeof(p));

//     p.r = (unsigned char)r;
//     p.g = (unsigned char)g;
//     p.b = (unsigned char)b;

//     return p;
// }

// void filter_optimized(void* args[]) {
//     unsigned int width = *(unsigned int*)args[0];
//     unsigned int height = *(unsigned int*)args[1];
//     Pixel* input = args[2];
//     Pixel* output = args[3];
//     float* filter = args[4];

//     for (int x = 0; x < width; ++x) {
//         for (int y = 0; y < height; ++y) {
//             Pixel* p = (Pixel*)malloc(sizeof(Pixel));
//             *p = convolution(input, x, y, width, height, filter);

//             output[x+y*width].r = p->r;
//             output[x+y*width].g = p->g;
//             output[x+y*width].b = p->b;

//             free(p);
//         }
//     }
// }