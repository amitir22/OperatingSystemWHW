#!/bin/make

ZIP_NAME = 315836569_203839733.zip
CC = gcc
CFLAGS = -g -Wall
LIBS = -lpthread

all: malloc_1 malloc_2 malloc_3 malloc_4

malloc_1:
	$(CC) $(CFLAGS) -o malloc_1 malloc_1.cpp

malloc_2:
	$(CC) $(CFLAGS) -o malloc_2 malloc_2.cpp

malloc_3:
	$(CC) $(CFLAGS) -o malloc_3 malloc_3.cpp

malloc_4:
	$(CC) $(CFLAGS) -o malloc_4 malloc_4.cpp

zip:
	zip $(ZIP_NAME) malloc_1.cpp malloc_2.cpp malloc_3.cpp malloc_4.cpp submitters.txt

clean:
	rm -f malloc_1 malloc_2 malloc_3 malloc_4 $(ZIP_NAME)
