/* -*- mode: c; coding: utf-8-unix -*- */
/**
 * @file textwin.h
 *
 * @author Naoyuki MORITA
 *
 */

#ifndef SPHERE_TEXTWIN_H_
#define SPHERE_TEXTWIN_H_

#ifdef __cplusplus
extern "C" {
#endif

extern GLuint load_font_image( void );
extern int draw_textwindow(GLuint tid, const lens_param_t * lens);

#ifdef __cplusplus
}
#endif
#endif /* SPHERE_TEXTWIN_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
