#########################################################
# File: Makefile
# Description: rudimentary makefile for Monoceros project
# Authors: Chris Spravka, Matthew Feidt, Chad Erdwins
# or just...
# gcc -o main main.c menu.h menu.c gamePlay.c gamePlay.h sprites.c sprites.h effects.c effects.h levels.c levels.h interfaces.h -lncurses -lm
#########################################################

#CFLAGS = -Wall -fpic -coverage -lm -std=c99 -D_XOPEN_SOURCE_EXTENDED
CFLAGS = -lncursesw -lm -Wall -std=gnu99

default: all

menu.o: menu.c menu.h
	gcc -c menu.c -g $(CFLAGS)

tcp_client.o: tcp_client.h tcp_client.c
	gcc -c tcp_client.c -g $(CFLAGS)

gamePlay.o: gamePlay.c gamePlay.h interfaces.h
	gcc -c gamePlay.c -g $(CFLAGS)

sprites.o: sprites.c sprites.h interfaces.h
	gcc -c sprites.c -g $(CFLAGS)

effects.o: effects.c effects.h interfaces.h
	gcc -c effects.c -g $(CFLAGS)

levels.o: levels.c levels.h interfaces.h planet.h
	gcc -c levels.c -g $(CFLAGS)

planet.o: levels.o planet.h planet.c interfaces.h
	gcc -c planet.c -g $(CFLAGS)

ai.o: ai.c ai.h interfaces.h
	gcc -c ai.c -g $(CFLAGS)

all: menu.o sprites.o gamePlay.o effects.o main.c levels.o planet.o ai.o interfaces.h tcp_client.o tcp_client.h tcp_server.c
	gcc -o main main.c -g menu.o sprites.o gamePlay.o effects.o levels.o planet.o ai.o tcp_client.o $(CFLAGS)
	gcc -o server tcp_server.c -g gamePlay.o tcp_client.o effects.o sprites.o levels.o menu.o planet.o ai.o $(CFLAGS)

server: tcp_server.c gamePlay.o tcp_client.o effects.o sprites.o levels.o menu.o planet.o ai.o
	gcc -o server tcp_server.c -g gamePlay.o tcp_client.o effects.o sprites.o levels.o menu.o planet.o ai.o $(CFLAGS)

clean:
	rm -f *.o main server
