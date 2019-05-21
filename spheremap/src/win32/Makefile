#!/usr/bin/make -f
# -*- makefile-gmake -*-

SDL_CONFIG = /usr/local/x86_64-w64-mingw32/bin/sdl-config
CC = x86_64-w64-mingw32-gcc
LD = x86_64-w64-mingw32-ld
OBJCOPY = x86_64-w64-mingw32-objcopy

CFLAGS = -Wall `$(SDL_CONFIG) --cflags` -I/usr/local/x86_64-w64-mingw32/include
LDFLAGS = `$(SDL_CONFIG) --libs` -lSDL_image -lopengl32

DEPDIR = ./.deps
SRCDIR = ..

COBJS = main.o textwin.o madoka.o

CDEPS = $(patsubst %.o, $(DEPDIR)/%.d, $(COBJS))
CSRCS = $(patsubst %.o, $(SRCDIR)/%.c, $(COBJS))

BINARIES = sphere.exe

.PHONY: all depend clean distclean

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@mkdir -p $(DEPDIR)/
	@echo $< '=>' $@
	@$(CC) $(CFLAGS) -MM -o $@ $< 

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(BINARIES)

depend: $(CDEPS)

ifeq ($(findstring $(MAKECMDGOALS), clean distclean),)
  -include $(CDEPS)
endif

resource/asciifont.tga: ../resource/asciifont.tga
	@mkdir -p resource
	cp $< $@

asciifont.o: resource/asciifont.tga
	$(OBJCOPY) -I binary -O pe-x86-64 -B i386 $< $@

sphere.exe: $(COBJS) asciifont.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(COBJS) asciifont.o resource/asciifont.tga $(BINARIES)

distclean:
	rm -rf $(DEPDIR) $(COBJS) asciifont.o resource/ $(BINARIES)

# EOF
