/* -*- mode: c; coding: utf-8-unix -*- */
/**
 * @file main.c
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

#include "common.h"
#include "vector.h"
#include "lens.h"
#include "madoka.h"
#include "textwin.h"


static void
toggle_lens_type(lens_param_t * lens, int fw)
{
	lens_type_t nxt_type = LENS_EQUISOLID;
	switch (lens->type) {
	case LENS_STEREOGRAPHIC:
		nxt_type = fw ? LENS_EQUIDISTANT : LENS_MADOKA;
		break;

	case LENS_EQUIDISTANT:
		nxt_type = fw ? LENS_EQUISOLID : LENS_STEREOGRAPHIC;
		break;

	case LENS_EQUISOLID:
		nxt_type = fw ? LENS_ORTHOGONAL : LENS_EQUIDISTANT;
		break;

	case LENS_ORTHOGONAL:
		nxt_type = fw ? LENS_MADOKA : LENS_EQUISOLID;
		break;

	case LENS_MADOKA:
		nxt_type = fw ? LENS_STEREOGRAPHIC : LENS_ORTHOGONAL;
		break;
	}

	lens->type = nxt_type;
}


static GLuint
LoadTexture(char_t * tex_name)
{
	GLuint tex_num;

	SDL_Surface * bmp_img;

	glGenTextures(1, &tex_num);

	bmp_img = (SDL_Surface *) IMG_Load(tex_name);
	
	if (bmp_img != NULL) {
		SDL_Surface * tex_img = SDL_CreateRGBSurface(SDL_SWSURFACE, 2048, 2048, 24,
													 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000);

		SDL_FillRect(tex_img, NULL, 0x00ff0000);

		{
			SDL_Rect dst_rect;
			dst_rect.x = (2048 - bmp_img->w)/2;
			dst_rect.y = (2048 - bmp_img->h)/2;
			dst_rect.w = bmp_img->w;
			dst_rect.h = bmp_img->h;
			SDL_BlitSurface(bmp_img, NULL, tex_img, &dst_rect);
		}

        glBindTexture(GL_TEXTURE_2D, tex_num);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if ((tex_img->format)->Amask) {
	        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_img->w, tex_img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_img->pixels);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_img->w, tex_img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_img->pixels);
		}

		SDL_FreeSurface (bmp_img);
        SDL_FreeSurface (tex_img);
	}
	else {
		fprintf(stderr, "Failed to load texture: %s\n", tex_name);
	}

	return tex_num;
}

#define  NDIV_V  (18)
#define  NDIV_H  (9)
#define  NSTRIPS (NDIV_V*NDIV_H)

#define  TEXSCALE_X (1.0/1.0*0.5)
#define  TEXSCALE_Y (1.0/1.0*0.5)

static int32_t
alloc_vertex_array_sphere(int32_t ** vcnts, vec3_t ** vertices, vec2_t ** coords)
{
	int32_t nvertices = (NDIV_V * (3+2*NDIV_V+1) / 2)*NDIV_H;

	vec3_t * vtxs;
	vec2_t * crds;
	int32_t * cnts;

	vtxs = malloc(sizeof(vec3_t)*nvertices);
	if (vtxs == NULL) {
		fprintf(stderr, "Failed to allocate memory...\n");
		return -1;
	}

	crds = malloc(sizeof(vec2_t)*nvertices);
	if (crds == NULL) {
		fprintf(stderr, "Failed to allocate memory...\n");
		free(vtxs);
		return -1;
	}

	cnts = malloc(sizeof(int32_t)*NSTRIPS);
	if (cnts == NULL) {
		fprintf(stderr, "Failed to allocate memory...\n");
		free(vtxs);
		free(crds);
		return -1;
	}

	*vcnts = cnts;
	*vertices = vtxs;
	*coords = crds;

	return nvertices;
}

static int32_t
alloc_vertex_array_wireframe(int32_t ** vcnts, vec3_t ** vertices)
{
	int32_t nvertices = (NDIV_V * (4+3*NDIV_V+1) / 2)*NDIV_H;

	vec3_t * vtxs;
	int32_t * cnts;

	vtxs = malloc(sizeof(vec3_t)*nvertices);
	if (vtxs == NULL) {
		fprintf(stderr, "Failed to allocate memory...\n");
		return -1;
	}

	cnts = malloc(sizeof(int32_t)*NSTRIPS);
	if (cnts == NULL) {
		fprintf(stderr, "Failed to allocate memory...\n");
		free(vtxs);
		return -1;
	}

	*vcnts = cnts;
	*vertices = vtxs;

	return nvertices;
}

static void
update_sphere_object(int32_t * nstrips, const lens_param_t * lens, int32_t cnts[], vec3_t vtxs[], vec2_t crds[])
{
	vec3_t vary[2*(NDIV_V+1)];
	vec2_t cary[2*(NDIV_V+1)];
	/* The number of vertices
	 *     Top
	 *     /\    2*1+1
	 *
	 *    /\/\   2*2+1
	 *
	 *   /\/\/\  2*3+1
	 * ...
	 *
	 */
	double r = 30.0;
	double cx = 0.5 + lens->center.x / 1024.0 * 0.5;
	double cy = 0.5 + lens->center.y / 1024.0 * 0.5;
	double t_r = lens->r / 1024.0;
	vec2_t center = vec2(cx, cy);
	
	int32_t scnt;
	int32_t vcnt;
	int32_t i, j, k;

	scnt = 0;
	vcnt = 0;
	for (i=0; i<NDIV_H; i++) {
		vary[0*(NDIV_V+1)+0] = vec3(0.0, 0.0, -r);
		cary[0*(NDIV_V+1)+0] = center;

		for (j=1; j<NDIV_V+1; j++) {
			int32_t slot_n = (j+0)&1;
			int32_t slot_p = (j+1)&1;
			double th_r = j*(1.0/NDIV_V);
			double theta = th_r*0.5*M_PI;
			double zn = cos(theta);
			double zz = -zn*r;
			double rr = sin(theta);

			for (k=0; k<j+1; k++) {
				double phi = (i*j+k)*2.0*M_PI*(1.0/NDIV_H)*(1.0/j);
				double xx = rr*cos(phi)*r;
				double yy = rr*sin(phi)*r;

				vec2_t tcr = vec2(cos(phi)*TEXSCALE_X*t_r, -sin(phi)*TEXSCALE_Y*t_r);
				vec2_t tc;

				switch (lens->type) {
				case LENS_STEREOGRAPHIC: {
					/* stereographic projection */
					tc = add2d(mult2d(rr*(1.0/(1.0+zn)), tcr), center);
					break;
				}

				case LENS_EQUIDISTANT: {
					/* equidistant projection */
					tc = add2d(mult2d(th_r, tcr), center);
					break;
				}

				case LENS_EQUISOLID: {
					/* equisolid projection */
					double esr = sin(theta*0.5) / sin(0.25*M_PI);
					tc = add2d(mult2d(esr, tcr), center);
					break;
				}

				case LENS_ORTHOGONAL: {
					/* orthogonal projection */
					tc = add2d(mult2d(rr, tcr), center);
					break;
				}

				case LENS_MADOKA: {
					/* MADOKA */
					double sr = madoka_theta_to_radius(theta);
					tc = add2d(mult2d(sr, tcr), center);
					break;
				}
				}

				vary[slot_n*(NDIV_V+1)+k] = vec3(xx, yy, zz);
				cary[slot_n*(NDIV_V+1)+k] = tc;
			}

			cnts[scnt] = 2*j+1;
			for (k=0; k<j; k++) {
				vtxs[vcnt] = vary[slot_n*(NDIV_V+1)+k];
				crds[vcnt] = cary[slot_n*(NDIV_V+1)+k];
				vcnt++;

				vtxs[vcnt] = vary[slot_p*(NDIV_V+1)+k];
				crds[vcnt] = cary[slot_p*(NDIV_V+1)+k];
				vcnt++;
			}
			vtxs[vcnt] = vary[slot_n*(NDIV_V+1)+j];
			crds[vcnt] = cary[slot_n*(NDIV_V+1)+j];
			vcnt++;

			scnt++;
		}		
	}	

	*nstrips = scnt;
}

static void
update_sphere_wireframe(int32_t * nstrips, int32_t cnts[], vec3_t vtxs[])
{
	vec3_t vary[2*(NDIV_V+1)];
	double r = 30.0f;
	int32_t scnt;
	int32_t vcnt;
	int32_t i, j, k;

	scnt = 0;
	vcnt = 0;
	for (i=0; i<NDIV_H; i++) {
		vary[0*(NDIV_V+1)+0] = vec3(0.0, 0.0, -r);

		for (j=1; j<NDIV_V+1; j++) {
			int32_t slot_n = (j+0)&1;
			int32_t slot_p = (j+1)&1;
			double theta = (NDIV_V-j)*0.5*M_PI*(1.0/NDIV_V);
			double zn = sin(theta);
			double zz = -zn*r;
			double rr = cos(theta);

			for (k=0; k<j+1; k++) {
				double phi = (i*j+k)*2.0*M_PI*(1.0/NDIV_H)*(1.0/j);
				double xx = rr*cos(phi)*r;
				double yy = rr*sin(phi)*r;
				vary[slot_n*(NDIV_V+1)+k] = vec3(xx, yy, zz);
			}

			cnts[scnt] = 3*j+1;
			for (k=0; k<j; k++) {
				vtxs[vcnt] = vary[slot_n*(NDIV_V+1)+k];
				vcnt++;

				vtxs[vcnt] = vary[slot_p*(NDIV_V+1)+k];
				vcnt++;
			}
			
			for (k=0; k<j+1; k++) {
				vtxs[vcnt] = vary[slot_n*(NDIV_V+1)+(j-k)];
				vcnt++;
			}

			scnt++;
		}		
	}

	*nstrips = scnt;
}

static void
set_viewangle(double fovY, int32_t width, int32_t height)
{
	GLdouble zNear = 1.0;
	GLdouble zFar  = 100.0;
	GLdouble fH = tan((fovY*0.5)/180.0*M_PI)*zNear;
	GLdouble fW = fH*((double)width)/((double)height);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity( );
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

static void
draw_sphere(GLuint tid, int32_t nstrips, int32_t vcnts[], vec3_t vertices[], vec2_t coords[])
{
	int32_t vc = 0;
	int32_t i;

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, tid);
	glVertexPointer(3, GL_DOUBLE, sizeof(double)*3, &vertices[0].x);
	glTexCoordPointer(2, GL_DOUBLE, sizeof(double)*2, &coords[0].x);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	for (i=0; i<nstrips; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, vc, vcnts[i]);
		vc += vcnts[i];
	}
}

static void
draw_wireframe(int32_t nstrips, int32_t wf_vcnts[], vec3_t wf_vtxs[])
{
	int32_t vc = 0;
	int32_t i;

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glVertexPointer(3, GL_DOUBLE, sizeof(double)*3, &wf_vtxs[0].x);
	glColor3f(0.0f, 1.0f, 0.0f);

	for (i=0; i<nstrips; i++) {
		glDrawArrays(GL_LINE_STRIP, vc, wf_vcnts[i]);
		vc += wf_vcnts[i];
	}
}

int
main(int argc, char ** argv)
{
	SDL_Surface * screen;
	SDL_Event event;
	int32_t quit = 0;

	const SDL_VideoInfo * info = NULL;
	int32_t width = 800;
	int32_t height = 600;
	int32_t bpp = 0;
	int32_t flags = 0;
	double fovY = 45.0;
	GLuint tid_sphere;
	GLuint tid_font;

	if (argc < 2) {
		fprintf(stderr, "filename required.\n");
		exit(-1);
	}

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		exit(-1);
	}

	atexit(SDL_Quit);

	info = SDL_GetVideoInfo();
	bpp = info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	flags = SDL_OPENGL;

	if((screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0) {
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		exit(-1);
	}
	
	SDL_WM_SetCaption("Fisheye photo mapping", NULL);

	/* Set projection matrix */
	set_viewangle(fovY, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	tid_sphere = LoadTexture(argv[1]);
	tid_font   = load_font_image( );

	{
		vec3_t * vertices;
		vec2_t * coords;
		vec3_t * wf_vtxs;
		int32_t * vcnts;
		int32_t * wf_vcnts;
		int32_t nstrips;
		int32_t frames = 0;
		GLfloat depth = 0.0f;
		int32_t wireframe = 0;
		int32_t textwin_en = 0;
		int32_t drag_p = 0;
		int32_t xorg, yorg;
		lens_param_t lens = {LENS_EQUIDISTANT, 1024.0, {0.0, 0.0}};
		float last_pitch = 0.0f;
		float pitch = 0.0f;
		float last_yaw = 0.0f;
		float yaw = 0.0f;

		if (alloc_vertex_array_sphere(&vcnts, &vertices, &coords) < 0) {
			exit(1);
		}

		if (alloc_vertex_array_wireframe(&wf_vcnts, &wf_vtxs) < 0) {
			exit(1);
		}

		update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
		update_sphere_wireframe(&nstrips, wf_vcnts, wf_vtxs);
		
		while (!quit) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_QUIT:
					quit = 1;
					break;

				case SDL_ACTIVEEVENT:
					if (drag_p && event.active.gain == 0 &&
						event.active.state == SDL_APPINPUTFOCUS) {
						drag_p = 0;
						last_pitch = pitch;
						last_yaw   = yaw;
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						drag_p = 1;
						xorg = event.button.x;
						yorg = event.button.y;
					}
					break;

				case SDL_MOUSEBUTTONUP:
					if (event.button.button == SDL_BUTTON_LEFT) {
						drag_p = 0;
						last_pitch = pitch;
						last_yaw   = yaw;
					}
					break;

				case SDL_MOUSEMOTION:
					if (drag_p) {
						int32_t delta_y = - event.motion.y + yorg;
						int32_t delta_x = - event.motion.x + xorg;

						pitch = last_pitch + delta_y * (45.0f/120.0f);
						if (pitch < -65.0f) {
							pitch = -65.0f;
						}
						else if (pitch > 65.0f) {
							pitch =  65.0f;
						}

						yaw   = last_yaw   + delta_x * (45.0f/120.0f);
						if (yaw   < -65.0f) {
							yaw   = -65.0f;
						}
						else if (yaw   > 65.0f) {
							yaw   =  65.0f;
						}
					}
					break;

				case SDL_KEYDOWN: {
					int32_t shift_p = ((event.key.keysym.mod & KMOD_SHIFT) != 0);

					switch( event.key.keysym.sym ){
					case SDLK_HOME:
						if (!drag_p) {
							depth = 0.0f;
							wireframe = 0;			
							last_pitch = 0.0f;
							pitch = 0.0f;
							last_yaw = 0.0f;
							yaw = 0.0f;
							lens.type = LENS_EQUIDISTANT;
							update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
							fovY = 45.0;
							set_viewangle(fovY, width, height);
						}
						break;

					case SDLK_ESCAPE:
					case SDLK_q:
						quit = 1;
						break;

					case SDLK_p: {
						printf("cx: %f\n", lens.center.x);
						printf("cy: %f\n", lens.center.y);
						printf("r : %f\n", lens.r);
						break;
					}

					case SDLK_w: {
						if (shift_p) {
							fovY += 2.0;
						}
						else {
							fovY += 0.5;
						}
						if (fovY > 75.0) {
							fovY = 75.0;
						}
						set_viewangle(fovY, width, height);
						break;
					}

					case SDLK_i: {
						textwin_en = 1 - textwin_en;
						break;
					}

					case SDLK_h: {
						if (shift_p) {
							lens.center.x -= 5.0;
						}
						else {
							lens.center.x -= 1.0;
						}
						update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
						break;
					}

					case SDLK_j: {
						if (shift_p) {
							lens.center.y += 5.0;
						}
						else {
							lens.center.y += 1.0;
						}
						update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
						break;
					}

					case SDLK_k: {
						if (shift_p) {
							lens.center.y -= 5.0;
						}
						else {
							lens.center.y -= 1.0;
						}
						update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
						break;
					}

					case SDLK_l: {
						if (shift_p) {
							lens.center.x += 5.0;
						}
						else {
							lens.center.x += 1.0;
						}
						update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
						break;
					}

					case SDLK_n: {
						if (shift_p) {
							fovY -= 2.0;
						}
						else {
							fovY -= 0.5;
						}
						if (fovY < 30.0) {
							fovY = 30.0;
						}
						set_viewangle(fovY, width, height);
						break;
					}

					case SDLK_r: {
						if (shift_p) {
							lens.r -= 5.0;
						}
						else {
							lens.r -= 1.0;
						}
						update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
						break;
					}

					case SDLK_e: {
						if (shift_p) {
							lens.r += 5.0;
						}
						else {
							lens.r += 1.0;
						}
						update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
						break;
					}

					case SDLK_TAB: {
						if (shift_p) {
							toggle_lens_type(&lens, 0);
						}
						else {
							toggle_lens_type(&lens, 1);
						}
						update_sphere_object(&nstrips, &lens, vcnts, vertices, coords);
						break;
					}

					case SDLK_SPACE:
						wireframe = 1 - wireframe;
						break;

					case SDLK_UP:
						depth += 5.0f;
						if (depth > 0.0f) {
							depth = 0.0f;
						}
						break;

					case SDLK_DOWN:
						depth -= 5.0f;
						if (depth < -50.0f) {
							depth = -50.0f;
						}
						break;

					default:
						/* do nothing */
						break;
					}
					break;
				}
				}
			}

			glClear(GL_COLOR_BUFFER_BIT);

			{
				/* Uint32 t = SDL_GetTicks( ); */

				glEnableClientState(GL_VERTEX_ARRAY);
				glLoadIdentity( );
				glTranslatef(0.0f, 0.0f, depth);
				glRotatef(yaw, 0.0f, 1.0f, 0.0f);
				glRotatef(pitch, 1.0f, 0.0f, 0.0f);

				if (wireframe) {
					draw_sphere(tid_sphere, nstrips, vcnts, vertices, coords);
					draw_wireframe(nstrips, wf_vcnts, wf_vtxs);
				}
				else {
					draw_sphere(tid_sphere, nstrips, vcnts, vertices, coords);
				}

				if (textwin_en) {
					draw_textwindow(tid_font, &lens);
				}
			}

			frames++;
			SDL_GL_SwapBuffers();
			SDL_Delay(10);
		}
	}
	
	return 0;
}

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
