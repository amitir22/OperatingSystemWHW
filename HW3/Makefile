#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
ZIP_NAME = 315836569_203839733.zip
OBJS = server.o request.o segel.o client.o message.o message_queue.o thread_pool.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread

.SUFFIXES: .c .o

all: server client output.cgi
	-mkdir -p public
	-cp output.cgi favicon.ico *.html *.txt public

server: server.o request.o segel.o message_queue.o message.o thread_pool.o
	$(CC) $(CFLAGS) -o server server.o request.o segel.o message_queue.o message.o \
		thread_pool.o $(LIBS)

client: client.o segel.o
	$(CC) $(CFLAGS) -o client client.o segel.o

output.cgi: output.c
	$(CC) $(CFLAGS) -o output.cgi output.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

zip:
	zip $(ZIP_NAME) *.c *.h Makefile submitters.txt
	zip -d $(ZIP_NAME) client.c output.c

clean:
	-rm -f $(OBJS) server client output.cgi $(ZIP_NAME)
	-rm -rf public
