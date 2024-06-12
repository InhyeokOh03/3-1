// Assignment 2 - Efficient C Programming
// System Programming, DGIST, Prof. Yeseong Kim
// 
// You will not turn in this file, so do not need to modify it.
// Read the provided instruction carefully.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmplib.h"
#include "perfenv.h"
#include "hw2.h"

//////////////////////////////////////////////////////////////////////////////
// Filter Definitions. Enjoy different filtered results! 
//////////////////////////////////////////////////////////////////////////////
float identity_filter[] = {
    0, 0, 0,
    0, 1, 0,
    0, 0, 0,
};

float edge_filter[] = {
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1,
};

float sharpen_filter[] = {
     0, -1,  0,
    -1,  5, -1,
     0, -1,  0,
};

float boxblur_filter[] = {
    1/9., 1/9., 1/9.,
    1/9., 1/9., 1/9.,
    1/9., 1/9., 1/9.,
};

float gaussian_filter[] = {
    1/16., 2/16., 1/16.,
    2/16., 4/16., 2/16.,
    1/16., 2/16., 1/16.,
};
////////////////////////////// End of Filters ////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Convolution baseline code to compare, which would be quite slow.
// You can find the same souce code in the provided proj.c,
// and have to modify the functions in "proj.c" to improve the performance.
//////////////////////////////////////////////////////////////////////////////
static Pixel convolution(
        Pixel* input, int x, int y, int width, int height, float* filter) {
    double r = 0;
    double g = 0;
    double b = 0;

    for (int dx = -1; dx < 2; ++dx) {
        for (int dy = -1; dy < 2; ++dy) {
            if ((y + dy) < 0 || (y + dy) >= height)
                continue;

            if ((x + dx) < 0 || (x + dx) >= width)
                continue;

            r += input[(x+dx)+(y+dy)*width].r * filter[(dx+1)+(dy+1)*3];
            g += input[(x+dx)+(y+dy)*width].g * filter[(dx+1)+(dy+1)*3];
            b += input[(x+dx)+(y+dy)*width].b * filter[(dx+1)+(dy+1)*3];
        }
    }

    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    
    Pixel p;
    memset(&p, 0, sizeof(p));

    p.r = (unsigned char)r;
    p.g = (unsigned char)g;
    p.b = (unsigned char)b;

    return p;
}

void filter_baseline(void* args[]) {
    unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = args[2];
    Pixel* output = args[3];
    float* filter = args[4];

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Pixel* p = (Pixel*)malloc(sizeof(Pixel));
            *p = convolution(input, x, y, width, height, filter);

            output[x+y*width].r = p->r;
            output[x+y*width].g = p->g;
            output[x+y*width].b = p->b;

            free(p);
        }
    }
}
////////////////////////////// End of Baseline ///////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Evaluation environment
//////////////////////////////////////////////////////////////////////////////
static inline int abs_diff(int v, int w) {
    int diff = v - w;
    if (diff < 0)
        return -1 * diff;
    return diff;
}

static inline int relaxed_diff(int v, int w) {
    int diff = abs_diff(v, w);
    if (diff <= 1)
        return 0;  // we allow the very small difference
    return diff;
}

static int compare(const Pixel* output1, const Pixel* output2,
        unsigned int width, unsigned int height) {
    int diff = 0;
    for (int i = 0; i < width * height; ++i) {
        diff += relaxed_diff(output1[i].r, output2[i].r);
        diff += relaxed_diff(output1[i].g, output2[i].g);
        diff += relaxed_diff(output1[i].b, output2[i].b);
    }

    return diff;
}

void run(unsigned int width, unsigned int height,
        Pixel* input, Pixel* output1, Pixel* output2) {
    // Create the argument list for the tested function
    void* args[6];
    args[0] = (void*)&width;
    args[1] = (void*)&height;
    args[2] = (void*)input;
    args[3] = (void*)output1;
    args[4] = (void*)edge_filter; // You can test different filters with here

    // If needed, you can execute the filter function as follows:
    //filter_baseline(args);
    //filter_optimized(args);

    // Run the code while collecting the execution time
    double time_baseline = measure((func_t)filter_baseline, args);
    args[3] = (void*)output2;
    double time_optimized = measure((func_t)filter_optimized, args);

    // We only allow up to 10 rgb value differences due to data precision 
    // If you believe we need more, please contact instructors
    if (compare(output1, output2, width, height) < 10) {
        // Print the evaluation result
        printf("Your speedup:\t%lf\n", time_baseline / time_optimized);
    }
}

int main(int argc, char** argv) {
    // Check command line arguments & print usage
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(1);
    }

    // Open the input BMP file
    BMP* bmp = load_bmp(argv[1]);
    printf("BMP file loaded: %d X %d\n", bmp->width, bmp->height);

    // Create two pixel buffer for the outputs
    Pixel* output1 = (Pixel*)malloc(
            bmp->width * bmp->height * sizeof(Pixel));
    Pixel* output2 = (Pixel*)malloc(
            bmp->width * bmp->height * sizeof(Pixel));

    // Run the provided baseline and your code, manipulating an output buffer
    run(bmp->width, bmp->height, bmp->pixels, output1, output2);

    // Write the output to a new file
    fill_bmp(bmp, output2);
    save_bmp(bmp, argv[2]);
   
    // Free memory
    free(output1);
    free(output2);
    unload_bmp(bmp);

    return 0;
}