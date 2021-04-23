# Makefile for Project 3
CFLAGS = -Wall 

client:  UDPClient.c die.c
	gcc $(CFLAGS) -o client UDPClient.c die.c

server:   UDPServer.c die.c
	gcc $(CFLAGS) -o server UDPServer.c die.c

UDPClient: client
	./client 172.31.146.118 31416

UDPServer: server
	./server 10029

runCS: client | 
	./client 127.0.0.1 10029

all: UDPServer runCS

clean:
	rm -f UDPClient UDPServer


