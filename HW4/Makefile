#!/bin/make

ZIP_NAME = 315836569_203839733.zip
CC = gcc
CFLAGS = -g -Wall
LIBS = -lpthread

all: malloc_1.o malloc_2.o malloc_3.o malloc_4.o

malloc_1.o:
	$(CC) $(CFLAGS) -c malloc_1.cpp

malloc_2.o:
	$(CC) $(CFLAGS) -c malloc_2.cpp

malloc_3.o:
	$(CC) $(CFLAGS) -c malloc_3.cpp

malloc_4.o:
	$(CC) $(CFLAGS) -c malloc_4.cpp

zip:
	zip $(ZIP_NAME) malloc_1.cpp malloc_2.cpp malloc_3.cpp malloc_4.cpp submitters.txt

clean:
	rm -f malloc_1.o malloc_2.o malloc_3.o malloc_4.o $(ZIP_NAME)
