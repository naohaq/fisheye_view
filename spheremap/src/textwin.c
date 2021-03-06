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
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl.h>

#include "common.h"
#include "vector.h"
#include "lens.h"
#include "textwin.h"

extern const uint8_t  _binary_resource_asciifont_tga_start[];
extern const uint8_t  _binary_resource_asciifont_tga_end[];
extern const uint32_t _binary_resource_asciifont_tga_size[];

GLuint
load_font_image( void )
{
	GLuint tex_num;
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

static void
draw_char(GLfloat x, GLfloat y, int c)
{
	const GLfloat ts = 1.0/512.0;
	const GLfloat tw = 11.0f;
	const GLfloat th = 24.0f;
	GLfloat tx0 = 10.0;
	GLfloat ty0 = 0.0;

	if (c >= 0x20 && c <= 0x7e) {
		GLfloat txoff = (GLfloat)(c & 0x0f)*tw*2;
		GLfloat tyoff = (GLfloat)(((c & 0x70) >> 4)-2)*th;
		tx0 += txoff;
		ty0 += tyoff;
	}

	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glTexCoord2f(tx0*ts, ty0*ts);
	glVertex3f(x, y, -2.0f);
	glTexCoord2f(tx0*ts, (ty0+th)*ts);
	glVertex3f(x, y-th, -2.0f);
	glTexCoord2f((tx0+tw)*ts, ty0*ts);
	glVertex3f(x+tw, y, -2.0f);
	glTexCoord2f((tx0+tw)*ts, (ty0+th)*ts);
	glVertex3f(x+tw, y-th, -2.0f);

	glEnd( );
}

static int
draw_string(GLfloat x, GLfloat y, const char * str)
{
	for (int32_t i=0; str[i] != 0; i+=1) {
		draw_char(x + ((GLfloat)i)*12.0f, y, str[i]);
	}
	return 0;
}

static const char *
projection_type_name(lens_type_t t)
{
	const char * name = "unknown";
	switch (t) {
	case LENS_STEREOGRAPHIC:
		name = "Stereographic";
		break;
	case LENS_EQUIDISTANT:
		name = "Equidistant";
		break;
	case LENS_EQUISOLID:
		name = "Equisolid";
		break;
	case LENS_ORTHOGONAL:
		name = "Orthogonal";
		break;
	case LENS_MADOKA:
		name = "MADOKA";
		break;
	}

	return name;
}

int
draw_textwindow(GLuint tid, const lens_param_t * lens)
{
	const GLdouble width  = (GLdouble)800;
	const GLdouble height = (GLdouble)600;

	const GLfloat x_left  = (GLfloat)-width *0.5 +  10.0;
	const GLfloat x_right = (GLfloat) width *0.5 -  10.0;
	const GLfloat y_top   = (GLfloat)-height*0.5 + 110.0;
	const GLfloat y_bottom= (GLfloat)-height*0.5 +  10.0;
	const GLfloat ox = x_left + 10.0;
	const GLfloat oy = y_top  -  5.0;

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
	glVertex3f(x_left , y_top   , -5.0f);
	glVertex3f(x_left , y_bottom, -5.0f);
	glVertex3f(x_right, y_top   , -5.0f);
	glVertex3f(x_right, y_bottom, -5.0f);
	glEnd( );

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tid);
	{
		char_t buf[128];
		snprintf(buf, 128, "Projection: %s\n", projection_type_name(lens->type));
		buf[127] = 0;
		draw_string(ox, oy, buf);
		snprintf(buf, 128, "Center: % 6.1f, % 6.1f", lens->center.x, lens->center.y);
		buf[127] = 0;
		draw_string(ox, oy-26.0f, buf);
		snprintf(buf, 128, "Radius: %6.1f", lens->r);
		buf[127] = 0;
		draw_string(ox, oy-26*2.0f, buf);
	}

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
