/* -*- mode: c; coding: utf-8-unix -*- */
/**
 * @file   vector_inline.h
 * @brief  Inline functions for operating 2d/3d vectors.
 *
 * @author Naoyuki MORITA
 */

#ifndef SPHEREMAP_VECTOR_INLINE_H_
#define SPHEREMAP_VECTOR_INLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

static inline vec2_t
vec2(double x, double y)
{
	vec2_t r = {x, y};
	return r;
}

static inline vec3_t
vec3(double x, double y, double z)
{
	vec3_t r = {x, y, z};
	return r;
}

static inline double
norm2d(vec2_t v)
{
	return sqrt(v.x*v.x+v.y*v.y);
}

static inline double
norm3d(vec3_t v)
{
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

static inline double
dot2d(vec2_t a, vec2_t b)
{
	return a.x*b.x+a.y*b.y;
}

static inline double
dot3d(vec3_t a, vec3_t b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

static inline vec3_t
cross3d(vec3_t a, vec3_t b)
{
	return vec3(a.y*b.z - a.z*b.y,
				a.z*b.x - a.x*b.z,
				a.x*b.y - a.y*b.x);
}

static inline vec2_t
add2d(vec2_t a, vec2_t b)
{
	return vec2(a.x+b.x, a.y+b.y);
}

static inline vec2_t
sub2d(vec2_t a, vec2_t b)
{
	return vec2(a.x-b.x, a.y-b.y);
}

static inline vec3_t
add3d(vec3_t a, vec3_t b)
{
	return vec3(a.x+b.x, a.y+b.y, a.z+b.z);
}

static inline vec3_t
sub3d(vec3_t a, vec3_t b)
{
	return vec3(a.x-b.x, a.y-b.y, a.z-b.z);
}

static inline vec2_t
mult2d(double a, vec2_t v)
{
	return vec2(a*v.x, a*v.y);
}

static inline vec3_t
mult3d(double a, vec3_t v)
{
	return vec3(a*v.x, a*v.y, a*v.z);
}

#ifdef __cplusplus
}
#endif
#endif /* SPHEREMAP_VECTOR_INLINE_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
