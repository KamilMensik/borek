// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "glm/ext/vector_float2.hpp"
#include <ostream>

/*
 *      This header files contains definitions of simd instructions for all supported platforms, so SIMD can be the same in code.
 * */

#define PLATFORM_X86
#ifdef PLATFORM_X86

#include <xmmintrin.h>

#define __M128I __m128i
#define __M128 __m128

#else
#error Only supporting X86 so far!
#endif

struct SimdVec4f {
        union {
                alignas(16)
                float data[4];
                struct { float x, y, z, w; };
                struct { float x1, y1, x2, y2; };
        };

        operator __M128() const;

        SimdVec4f() = default;
        SimdVec4f(__M128 val);
        SimdVec4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        friend std::ostream& operator <<(std::ostream& ost, const SimdVec4f&);
};

struct SimdVec4i {
        union {
                alignas(16) int data[4];
                struct { int x, y, z, w; };
                struct { int x1, y1, x2, y2; };
        };

        operator __M128I() const;

        SimdVec4i() = default;
        SimdVec4i(__M128I val);
        SimdVec4i(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}

        friend std::ostream& operator <<(std::ostream& ost, const SimdVec4i&);
};

__M128I simd_min4i(__M128I a, __M128I b);
__M128I simd_max4i(__M128I a, __M128I b);
__M128I simd_clamp4i(__M128I val, __M128I min, __M128I max);
__M128I simd_add4i(__M128I a, __M128I b);
__M128I simd_sub4i(__M128I a, __M128I b);
__M128I simd_mul4i(__M128I a, __M128I b);
__M128I simd_create4i(int a, int b, int c, int d);
__M128I simd_ftoi4f(__M128 val);
__M128I simd_zero4i();
SimdVec4i simd_store4i(__M128I val);
__M128I simd_load4i(const SimdVec4i& vec);

__M128I simd_scalar4i(int from);

__M128 simd_create4f(float x, float y, float z, float w);
__M128 simd_mul4f(__M128 a, __M128 b);
__M128 simd_sub4f(__M128 a, __M128 b);
__M128 simd_add4f(__M128 a, __M128 b);

SimdVec4f simd_store4f(__M128 val);
__M128 simd_load4f(const SimdVec4f& vec);
__M128 simd_loadu4f(const SimdVec4f& vec);
__M128 simd_loadu4f(const float* vec);
__M128 simd_itof4i(__M128I val);
__M128 simd_scalar4f(float from);

__M128 simd_abs(__M128 a);

bool simd_rect_intersect4f(__M128 a, __M128 b);

__M128 simd_flip4f(__M128 a);
