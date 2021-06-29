#!/bin/make

ZIP_NAME = 315836569_203839733.zip
CC = g++
CFLAGS = -g -Wall
LIBS = -lpthread

all: malloc_1.o malloc_2.o malloc_3.o malloc_4.o

malloc_1.o: malloc_1.cpp
	$(CC) $(CFLAGS) -c malloc_1.cpp

malloc_2.o: malloc_2.cpp
	$(CC) $(CFLAGS) -c malloc_2.cpp

malloc_3.o: malloc_3.cpp
	$(CC) $(CFLAGS) -c malloc_3.cpp

malloc_4.o: malloc_4.cpp
	$(CC) $(CFLAGS) -c malloc_4.cpp

tests: tests.cpp
	$(CC) $(CFLAGS) -o tests tests.cpp

zip: malloc_1.cpp malloc_2.cpp malloc_3.cpp malloc_4.cpp submitters.txt
	zip $(ZIP_NAME) malloc_1.cpp malloc_2.cpp malloc_3.cpp malloc_4.cpp submitters.txt

clean:
	rm -f malloc_1.o malloc_2.o malloc_3.o malloc_4.o $(ZIP_NAME)
