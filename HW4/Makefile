#!/bin/make

ZIP_NAME = 315836569_203839733.zip
CC = gcc
CFLAGS = -g -Wall
LIBS = -lpthread

all: malloc_1 malloc_2 malloc_3 malloc_4

malloc_1:
	$(CC) $(CFLAGS) -o malloc_1 malloc_1.c

malloc_2:
	$(CC) $(CFLAGS) -o malloc_2 malloc_2.c

malloc_3:
	$(CC) $(CFLAGS) -o malloc_3 malloc_3.c

malloc_4:
	$(CC) $(CFLAGS) -o malloc_4 malloc_4.c

zip:
	zip $(ZIP_NAME) malloc_1.c malloc_2.c malloc_3.c malloc_4.c submitters.txt

clean:
	rm -f malloc_1 malloc_2 malloc_3 malloc_4 $(ZIP_NAME)
