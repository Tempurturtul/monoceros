#########################################################
# File: Makefile
# Description: rudimentary makefile for Monoceros project
# Author: cspravka
# or just...
# gcc -o main main.c menu.h menu.c gamePlay.c gamePlay.h sprites.c sprites.h effects.c effects.h -lncurses
#########################################################

#CFLAGS = -Wall -fpic -coverage -lm -std=c99
CFLAGS = -lncurses -Wall

menu.o: menu.c menu.h
	gcc -c menu.c -g $(CFLAGS)

gameplay.o: gameplay.c gameplay.h
	gcc -c gameplay.c -g $(CFLAGS)
	
sprites.o: sprites.c sprites.h
	gcc -c sprites.c -g $(CFLAGS)

effects.o: effects.c effects.h
	gcc -c effects.c -g $(CFLAGS)

all: menu.o sprites.o gameplay.o effects.o main.c 
	gcc -o main main.c -g menu.o sprites.o gameplay.o effects.o $(CFLAGS)

clean:
	rm -f *.o main.exe
