// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

/*
 *      This header files contains definitions of simd instructions for all supported platforms, so SIMD can be the same in code.
 * */

#include "glm/ext/vector_float2.hpp"
#define PLATFORM_X86
#ifdef PLATFORM_X86

#include <xmmintrin.h>

#define M128I __m128i
#define M128 __m128

struct alignas(16) int4 {
        int x, y, z, w;
        inline operator int*() { return &x; }
};

struct alignas(16) float4 {
        float x, y, z, w;
        inline operator float*() { return &x; }
};

#else
#error Only supporting X86 so far!
#endif

// M128I

// Creation methods
M128I simd_scalar4i(int from);
M128I simd_create4i(int a, int b, int c, int d);
M128I simd_zero4i();
int4 simd_dump4i(M128I val);

// Convertion methods
M128I simd_ftoi4f(M128 val);

// Comparison operators
M128I simd_min4i(M128I a, M128I b);
M128I simd_max4i(M128I a, M128I b);

M128I simd_clamp4i(M128I val, M128I min, M128I max);
// Arithmetics
M128I simd_add4i(M128I a, M128I b);
M128I simd_sub4i(M128I a, M128I b);
M128I simd_mul4i(M128I a, M128I b);


// M128

// Creation methods
M128 simd_create4f(float x, float y, float z, float w);
M128 simd_scalar4f(float from);
M128 simd_zero4f();
float4 simd_dump4f(M128 val);

// Conversion methods
M128 simd_itof4i(M128I val);

// Arithmetic methods
M128 simd_mul4f(M128 a, M128 b);
M128 simd_sub4f(M128 a, M128 b);
M128 simd_add4f(M128 a, M128 b);
M128 simd_div4f(M128 a, M128 b);
M128 simd_abs(M128 a);
float simd_fmin4f(M128 val);
float simd_fmax4f(M128 val);

// Misc
M128 simd_flip4f(M128 a);

// Misc
bool simd_rect_intersect4f(M128 a, M128 b);

void simd_rotate_rect(const glm::vec2& pos, const glm::vec2& size,
                      const glm::vec2& origin, float degrees,
                      float4& xout, float4& yout);
