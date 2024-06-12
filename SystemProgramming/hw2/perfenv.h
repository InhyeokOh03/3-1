// Assignment 2 - Efficient C Programming
// System Programming, DGIST, Prof. Yeseong Kim
// 
// You will not turn in this file, so do not need to modify it.
// Read the provided instruction carefully.
//
// - Compute the number of cycles for the function f with given parameters

#ifndef _PERF_ENV_H_
#define _PERF_ENV_H_

typedef void (*func_t)(void*);
double measure(func_t f, void* params[]);

#endif
