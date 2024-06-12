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