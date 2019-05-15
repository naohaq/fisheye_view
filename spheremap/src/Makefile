#!/usr/bin/make -f
# -*- makefile-gmake -*-

SDL_CONFIG = /usr/bin/sdl-config

CFLAGS  = `$(SDL_CONFIG) --cflags`
LDFLAGS = `$(SDL_CONFIG) --libs` -lSDL_image -lm -lGL

.PHONY: all depend clean distclean

all: sphere

main.o: main.c
resource/asciifont.o: resource/asciifont.tga
	objcopy -I binary -O elf64-x86-64 -B i386 $< $@

sphere: main.o resource/asciifont.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f main.o asciifont.o sphere

