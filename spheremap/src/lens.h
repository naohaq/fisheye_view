/* -*- mode: c; coding: utf-8-unix -*- */
/**
 * @file lens.h
 *
 */

#ifndef SPHERE_LENS_H_
#define SPHERE_LENS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	LENS_STEREOGRAPHIC = 0,
	LENS_EQUIDISTANT,
	LENS_EQUISOLID,
	LENS_ORTHOGONAL,
	LENS_MADOKA,
} lens_type_t;

typedef struct {
	lens_type_t type;
	double r;
	vec2_t center;
} lens_param_t;


#ifdef __cplusplus
}
#endif
#endif /* SPHERE_LENS_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
