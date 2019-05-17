#!/usr/bin/make -f
# -*- makefile-gmake -*-

SDL_CONFIG = /usr/bin/sdl-config

CFLAGS  = `$(SDL_CONFIG) --cflags`
LDFLAGS = `$(SDL_CONFIG) --libs` -lSDL_image -lm -lGL

DEPDIR = ./.deps

COBJS = main.o textwin.o

CDEPS = $(patsubst %.o, $(DEPDIR)/%.d, $(COBJS))
CSRCS = $(patsubst %.o, %.c, $(COBJS))

BINARIES = sphere

.PHONY: all depend clean distclean

$(DEPDIR)/%.d: %.c
	@mkdir -p $(DEPDIR)/
	@echo $< '=>' $@
	@$(CC) $(CFLAGS) -MM -o $@ $< 

all: $(BINARIES)

depend: $(CDEPS)

ifeq ($(findstring $(MAKECMDGOALS), clean distclean),)
  -include $(CDEPS)
endif

resource/asciifont.o: resource/asciifont.tga
	objcopy -I binary -O elf64-x86-64 -B i386 $< $@

sphere: $(COBJS) resource/asciifont.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(COBJS) resource/asciifont.o $(BINARIES)

distclean:
	rm -rf $(DEPDIR) $(COBJS) resource/asciifont.o $(BINARIES)

# EOF
