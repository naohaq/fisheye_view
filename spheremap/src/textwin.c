/* -*- mode: c; coding: utf-8-unix -*- */
/**
 * @file textwin.c
 * 
 * @author Naoyuki MORITA
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl.h>

typedef char char_t;

#include "vector.h"

#include "textwin.h"

extern const uint8_t  _binary_resource_asciifont_tga_start[];
extern const uint8_t  _binary_resource_asciifont_tga_end[];
extern const uint32_t _binary_resource_asciifont_tga_size[];

GLuint
load_font_image( void )
{
	GLuint tex_num;
	SDL_Surface * fnt_img;
	SDL_RWops * ops;
	int imgsize = _binary_resource_asciifont_tga_end - _binary_resource_asciifont_tga_start;

	glGenTextures(1, &tex_num);

	ops = SDL_RWFromConstMem(_binary_resource_asciifont_tga_start, imgsize);
	if (ops != NULL) {
		SDL_Surface * fnt_img;
		fnt_img = IMG_LoadTGA_RW(ops);

		if (fnt_img != NULL) {
			SDL_Surface * tex_img = SDL_CreateRGBSurface(SDL_SWSURFACE, 512, 512, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

			SDL_SetAlpha(fnt_img, 0, 255);

			SDL_FillRect(tex_img, NULL, 0x00000000);

			SDL_BlitSurface(fnt_img, NULL, tex_img, NULL);

			glBindTexture(GL_TEXTURE_2D, tex_num);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_img->w, tex_img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_img->pixels);

			SDL_FreeSurface(tex_img);
			SDL_FreeSurface(fnt_img);
		}
		else {
			fprintf(stderr, "IMG_Load_RW: %s\n", SDL_GetError( ));
		}
	}
	else {
		fprintf(stderr, "SDL_RWFromConstMem: %s\n", SDL_GetError( ));
	}

	return tex_num;
}

int
draw_textwindow(GLuint tid)
{
	const GLdouble width  = (GLdouble)800;
	const GLdouble height = (GLdouble)600;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix( );
	glLoadIdentity( );
	glOrtho(-width*0.5, width*0.5, -height*0.5, height*0.5, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity( );

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
	glVertex3f((GLfloat)-width*0.5+10.0, (GLfloat)-height*0.25    , -2.0f);
	glVertex3f((GLfloat)-width*0.5+10.0, (GLfloat)-height*0.5+10.0, -2.0f);
	glVertex3f((GLfloat) width*0.5-10.0, (GLfloat)-height*0.25    , -2.0f);
	glVertex3f((GLfloat) width*0.5-10.0, (GLfloat)-height*0.5+10.0, -2.0f);
	glEnd( );

	glMatrixMode(GL_PROJECTION);
	glPopMatrix( );
	glMatrixMode(GL_MODELVIEW);

	return 0;
}


/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
