# Makefile for Project 2
CFLAGS = -Wall -Og

all:	forker  sigs  ipc

forker:  forker.c dieWithError.c
	gcc $(CFLAGS) -o forker forker.c dieWithError.c

sigs:   sigs.c dieWithError.c
	gcc $(CFLAGS) -o sigs sigs.c dieWithError.c

ipc:	ipc.c dieWithError.c
	gcc $(CFLAGS) -o ipc ipc.c dieWithError.c
