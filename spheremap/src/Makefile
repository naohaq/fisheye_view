#!/usr/bin/make -f
# -*- makefile-gmake -*-

SDL_CONFIG = /usr/bin/sdl-config

CFLAGS  = `$(SDL_CONFIG) --cflags`
LDFLAGS = `$(SDL_CONFIG) --libs` -lSDL_image -lm -lGL

.PHONY: all depend clean distclean

all: sphere

main.o: main.c

sphere: main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f main.o sphere

