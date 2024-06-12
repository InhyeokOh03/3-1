CC = gcc
CFLAGS = -Wall -O0 -fno-tree-vectorize
LIBS = -lm

OBJS = bmplib.o perfenv.o main.o hw2.o

all: bmpfilter

bmpfilter: $(OBJS) bmplib.h perfenv.h hw2.h
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o bmpfilter

clean: 
	-rm -f $(OBJS) bmpfilter
