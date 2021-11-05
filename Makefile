#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o stems.o clientGet.o clientPost.o alarmClient.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server clientPost clientGet dataGet.cgi dataPost.cgi alarmClient

server: server.o request.o stems.o
	$(CC) $(CFLAGS) -w -o server server.o request.o stems.o $(LIBS)

clientGet: clientGet.o stems.o
	$(CC) $(CFLAGS) -w -o clientGet clientGet.o stems.o

clientPost: clientPost.o stems.o
	$(CC) $(CFLAGS) -w -o clientPost clientPost.o stems.o $(LIBS)

dataGet.cgi: dataGet.c stems.h
	$(CC) $(CFLAGS) -w -o dataGet.cgi dataGet.c -lmysqlclient

dataPost.cgi: dataPost.c stems.h
	$(CC) $(CFLAGS) -w -o dataPost.cgi dataPost.c stems.o -lmysqlclient

alarmClient: alarmClient.o stems.o
	$(CC) $(CFLAGS) -w -o alarmClient alarmClient.o stems.o $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -w -o $@ -c $<

server.o: stems.h request.h
clientGet.o: stems.h
clientPost.o: stems.h
alarmClient.o: stems.h

clean:
	-rm -f $(OBJS) server clientPost clientGet dataGet.cgi dataPost.cgi alarmClient
