#########################################################
# File: Makefile
# Description: rudimentary makefile for Monoceros project
# Authors: Chris Spravka, Matthew Feidt
# or just...
# gcc -o main main.c menu.h menu.c gamePlay.c gamePlay.h sprites.c sprites.h effects.c effects.h -lncurses
#########################################################

#CFLAGS = -Wall -fpic -coverage -lm -std=c99
CFLAGS = -lncurses -Wall

default: all

menu.o: menu.c menu.h
	gcc -c menu.c -g $(CFLAGS)

gamePlay.o: gamePlay.c gamePlay.h
	gcc -c gamePlay.c -g $(CFLAGS)
	
sprites.o: sprites.c sprites.h
	gcc -c sprites.c -g $(CFLAGS)

effects.o: effects.c effects.h
	gcc -c effects.c -g $(CFLAGS)

all: menu.o sprites.o gamePlay.o effects.o main.c 
	gcc -o main main.c -g menu.o sprites.o gamePlay.o effects.o $(CFLAGS)

clean:
	rm -f *.o main