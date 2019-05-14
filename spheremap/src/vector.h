/* -*- mode: c; coding: utf-8-unix -*- */
/**
 * @file   vector.h
 * @brief  Vector(2d/3d) type definitions.
 *
 * @author Naoyuki MORITA
 */

#ifndef SPHEREMAP_VECTOR_H_
#define SPHEREMAP_VECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	double x;
	double y;
	double z;
} vec3_t;

typedef struct {
	double x;
	double y;
} vec2_t;

#include "vector_inline.h"

#ifdef __cplusplus
}
#endif
#endif /* SPHEREMAP_VECTOR_H_ */

/*
 * Local Variables:
 * indent-tabs-mode: t
 * tab-width: 4
 * End:
 */
