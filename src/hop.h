#pragma once

// The shading pipeline uses floats
// This define determines if the geometry pipelines
// should use floats or doubles
#undef REAL_IS_DOUBLE

#ifdef REAL_IS_DOUBLE
    #define BBOX_SIMD_ISECT
#endif

#define RAND_HARDWARE

#define MIN_PRIMS_PER_LEAF 8
#define NUM_SAH_SPLITS 16
#define BVH_TRAV_COST Real(0.25)
#define NUM_AO_RAYS 5

#define TILES_SPIRAL

#define AO_BACKGROUND Spectrum(0.0f, 0.0f, 0.0f)

#define likely(x) __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)

#define ALIGN(x) __attribute__((aligned((x))))

#define NOOPTIMIZE __attribute__((optimize("O0")))
