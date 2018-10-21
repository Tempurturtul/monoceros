#########################################################
# File: Makefile
# Description: rudimentary makefile for Monoceros project
# Authors: Chris Spravka, Matthew Feidt, Chad Erdwins
# or just...
# gcc -o main main.c menu.h menu.c gamePlay.c gamePlay.h sprites.c sprites.h effects.c effects.h levels.c levels.h interfaces.h -lncurses -lm
#########################################################

#CFLAGS = -Wall -fpic -coverage -lm -std=c99
CFLAGS = -lncurses -Wall -lm #added -lm here to solve compile issue

default: all

menu.o: menu.c menu.h
	gcc -c menu.c -g $(CFLAGS)

gamePlay.o: gamePlay.c gamePlay.h interfaces.h
	gcc -c gamePlay.c -g $(CFLAGS)

sprites.o: sprites.c sprites.h interfaces.h
	gcc -c sprites.c -g $(CFLAGS)

effects.o: effects.c effects.h interfaces.h
	gcc -c effects.c -g $(CFLAGS)

levels.o: levels.c levels.h interfaces.h
	gcc -c levels.c -g $(CFLAGS)

ai.o: ai.c ai.h interfaces.h
	gcc -c ai.c -g $(CFLAGS)

all: menu.o sprites.o gamePlay.o effects.o main.c levels.o ai.o interfaces.h
	gcc -o main main.c -g menu.o sprites.o gamePlay.o effects.o levels.o ai.o $(CFLAGS)

clean:
	rm -f *.o main
