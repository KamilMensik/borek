// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <pmmintrin.h>
#include <smmintrin.h>

#include "Include/Platform/SIMD.h"
#include "Include/Core.h"
#include <xmmintrin.h>
#include <cmath>

std::ostream& operator <<(std::ostream& ost, const SimdVec4f& v)
{
        return ost << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]"; 
}

std::ostream& operator <<(std::ostream& ost, const SimdVec4i& v)
{
        return ost << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]"; 
}
SimdVec4f::operator __M128() const
{
        return _mm_load_ps(data);
}

SimdVec4f::SimdVec4f(__M128 val)
{
        _mm_store_ps(data, val);
}
SimdVec4i::operator __M128I() const
{
        return _mm_load_si128(RCAST<const __M128I*>(data));
}

SimdVec4i::SimdVec4i(__M128I val)
{
        _mm_store_si128(RCAST<__M128I*>(data), val);
}

__M128I simd_min4i(__M128I a, __M128I b)
{
        return _mm_min_epi32(a, b);
}

__M128I simd_max4i(__M128I a, __M128I b)
{
        return _mm_max_epi32(a, b);
}

__M128I simd_clamp4i(__M128I val, __M128I min, __M128I max)
{
        __M128I v = simd_min4i(val, max);
        return simd_max4i(v, min);
}

__M128I simd_add4i(__M128I a, __M128I b)
{
        return _mm_add_epi32(a, b);
}

__M128I simd_sub4i(__M128I a, __M128I b)
{
        return _mm_sub_epi32(a, b);
}

__M128I simd_mul4i(__M128I a, __M128I b)
{
        return _mm_mul_epi32(a, b);
}

__M128I simd_create4i(int x, int y, int z, int w)
{
        return _mm_set_epi32(w, z, y, x);
}

__M128I simd_ftoi4f(__M128 val)
{
        return _mm_cvttps_epi32(val);
}

__M128I simd_zero4i()
{
        return _mm_setzero_si128();
}

SimdVec4i simd_store4i(__M128I val)
{
        SimdVec4i res;
        _mm_store_si128(RCAST<__M128I*>(&res.data), val);
        return res;
}

__M128I simd_load4i(const SimdVec4i& vec)
{
        return _mm_load_si128(RCAST<const __M128I*>(vec.data));
}

__M128I simd_scalar4i(int from)
{
        return _mm_set1_epi32(from);
}

__M128 simd_create4f(float x, float y, float z, float w)
{
        return _mm_set_ps(w, z, y, x);
}

__M128 simd_mul4f(__M128 a, __M128 b)
{
        return _mm_mul_ps(a, b);
}

__M128 simd_sub4f(__M128 a, __M128 b)
{
        return _mm_sub_ps(a, b);
}

__M128 simd_add4f(__M128 a, __M128 b)
{
        return _mm_add_ps(a, b);
}

SimdVec4f simd_store4f(__M128 val)
{
        SimdVec4f res;
        _mm_store_ps(res.data, val);
        return res;
}

__M128 simd_load4f(const SimdVec4f& vec)
{
        return _mm_load_ps(vec.data);
}

__M128 simd_loadu4f(const SimdVec4f& vec)
{
        return _mm_loadu_ps(vec.data);
}

__M128 simd_loadu4f(const float* vec)
{
        return _mm_loadu_ps(vec);
}

__M128 simd_itof4i(__M128I val)
{
        return _mm_cvtepi32_ps(val);
}

__M128 simd_scalar4f(float from)
{
        return _mm_set1_ps(from);
}

__M128 simd_abs(__M128 a)
{
        const __M128 minus_one = simd_scalar4f(-1.0f);
        return _mm_max_ps(a, _mm_mul_ps(a, minus_one));
}

bool simd_rect_intersect4f(__M128 a, __M128 b)
{
        __M128 r1 = _mm_shuffle_ps(a, b, _MM_SHUFFLE(1, 0, 1, 0));
        __M128 r2 = _mm_shuffle_ps(b, a, _MM_SHUFFLE(3, 2, 3, 2));
        __M128I res = _mm_castps_si128(_mm_cmple_ps(r1, r2));

        return _mm_movemask_ps(_mm_castsi128_ps(res)) == 0xF;
}

__M128 simd_flip4f(__M128 a)
{
        return _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));
}
