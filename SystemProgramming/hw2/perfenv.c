// Assignment 2 - Efficient C Programming
// System Programming, DGIST, Prof. Yeseong Kim
// 
// You will not turn in this file, so do not need to modify it.
// Read the provided instruction carefully.
//
// - Measure the execution time for a function f with given parameters.
// - Some implementations come with the computer system programming book.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <sys/times.h>

#include "perfenv.h"

inline static double get_nano_time() {
	struct timespec ts = {0,0};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + 1.0e-9*ts.tv_nsec;
}

#define K 3
#define MAXSAMPLES 20
#define WARM_COUNT 2
#define EPSILON 0.01 
#define CLEAR_CACHE 1
#define CACHE_BYTES (1<<27) // 127MB
#define CACHE_BLOCK 32

static int kbest = K;
static int clear_cache = CLEAR_CACHE;
static int maxsamples = MAXSAMPLES;
static double epsilon = EPSILON;
static int cache_bytes = CACHE_BYTES;
static int cache_block = CACHE_BLOCK;

static int *cache_buf = NULL;
static double *values = NULL;
static int samplecount = 0;

/* Start new sampling process */
static void init_sampler() {
	if (values)
		free(values);
	values = calloc(kbest, sizeof(double));
	samplecount = 0;
}

/* Add new sample.  */
static void add_sample(double val)
{
	int pos = 0;
	if (samplecount < kbest) {
		pos = samplecount;
		values[pos] = val;
	} else if (val < values[kbest-1]) {
		pos = kbest-1;
		values[pos] = val;
	}
	samplecount++;
	/* Insertion sort */
	while (pos > 0 && values[pos-1] > values[pos]) {
		double temp = values[pos-1];
		values[pos-1] = values[pos];
		values[pos] = temp;
		pos--;
	}
}

/* Have kbest minimum measurements converged within epsilon? */
static int has_converged() {
	return
		(samplecount >= kbest) &&
		((1 + epsilon)*values[0] >= values[kbest-1]);
}

/* Code to clear cache */
static volatile int sink = 0;

static void clear()
{
	int x = sink;
	int *cptr, *cend;
	int incr = cache_block/sizeof(int);
	if (!cache_buf) {
		cache_buf = malloc(cache_bytes);
		if (!cache_buf) {
			fprintf(stderr, "Fatal error.  Malloc returned null when trying to clear cache\n");
			exit(1);
		}
	}
	cptr = (int *) cache_buf;
	cend = cptr + cache_bytes/sizeof(int);
	while (cptr < cend) {
		x += *cptr;
		cptr += incr;
	}
	sink = x;
}

double measure(func_t f, void *params[]) {
	double result;
	init_sampler();
	int i;
	for (i = 0; i < WARM_COUNT; ++i)
		f(params);

	i = 0;
	do {
		double t;
		if (clear_cache)
			clear();
		t = get_nano_time();
		f(params);
		add_sample(get_nano_time() - t);
		printf("Trial %d\n", i++);
	} while (!has_converged() && samplecount < maxsamples);
	result = values[0];
	return result;  
}
