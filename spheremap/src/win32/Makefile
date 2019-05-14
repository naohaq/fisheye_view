#!/usr/bin/make -f
# -*- makefile-gmake -*-

SDL_CONFIG = /usr/local/i686-w64-mingw32/bin/sdl-config
CC = i686-w64-mingw32-gcc
LD = i686-w64-mingw32-gcc

CFLAGS = `$(SDL_CONFIG) --cflags` -I/usr/local/i686-w64-mingw32-gcc/include
LDFLAGS = `$(SDL_CONFIG) --libs` -lSDL_image -lopengl32

.PHONY: all depend clean distclean

%.o: ../%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: sphere.exe

main.o: ../main.c ../vector.h ../vector_inline.h

sphere.exe: main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f sphere.exe main.o


