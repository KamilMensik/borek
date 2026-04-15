// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include "glm/trigonometric.hpp"
#include <algorithm>
#include <cmath>
#include <emmintrin.h>
#include <immintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>
#include <xmmintrin.h>

#include "Include/Platform/SIMD.h"

M128I simd_min4i(M128I a, M128I b)
{
        return _mm_min_epi32(a, b);
}

M128I simd_max4i(M128I a, M128I b)
{
        return _mm_max_epi32(a, b);
}

M128I simd_clamp4i(M128I val, M128I min, M128I max)
{
        M128I v = simd_min4i(val, max);
        return simd_max4i(v, min);
}

M128I simd_add4i(M128I a, M128I b)
{
        return _mm_add_epi32(a, b);
}

M128I simd_sub4i(M128I a, M128I b)
{
        return _mm_sub_epi32(a, b);
}

M128I simd_mul4i(M128I a, M128I b)
{
        return _mm_mul_epi32(a, b);
}

M128I simd_create4i(int x, int y, int z, int w)
{
        return _mm_set_epi32(w, z, y, x);
}

M128I simd_ftoi4f(M128 val)
{
        return _mm_cvttps_epi32(val);
}

M128I simd_zero4i()
{
        return _mm_setzero_si128();
}

M128I simd_scalar4i(int from)
{
        return _mm_set1_epi32(from);
}

M128 simd_create4f(float x, float y, float z, float w)
{
        return _mm_set_ps(w, z, y, x);
}

int4 simd_dump4i(M128I val)
{
        int4 res;
        _mm_store_si128(RCAST<M128I*>(&res), val);
        return res;
}

M128 simd_mul4f(M128 a, M128 b)
{
        return _mm_mul_ps(a, b);
}

M128 simd_sub4f(M128 a, M128 b)
{
        return _mm_sub_ps(a, b);
}

M128 simd_add4f(M128 a, M128 b)
{
        return _mm_add_ps(a, b);
}

M128 simd_div4f(M128 a, M128 b)
{
        return _mm_div_ps(a, b);
}

M128 simd_itof4i(M128I val)
{
        return _mm_cvtepi32_ps(val);
}

float simd_fmin4f(M128 val)
{
        float4 res = simd_dump4f(val);

        const float m1 = std::min(res.x, res.y);
        const float m2 = std::min(res.z, res.w);
        return std::min(m1, m2);
}

float simd_fmax4f(M128 val)
{
        float4 res = simd_dump4f(val);

        const float m1 = std::max(res.x, res.y);
        const float m2 = std::max(res.z, res.w);
        return std::max(m1, m2);
}

M128 simd_scalar4f(float from)
{
        return _mm_set1_ps(from);
}

M128 simd_zero4f()
{
        return _mm_setzero_ps();
}

float4 simd_dump4f(M128 val)
{
        float4 res;
        _mm_store_ps(res, val);
        return res;
}

M128 simd_abs(M128 a)
{
        const M128 minus_one = simd_scalar4f(-1.0f);
        return _mm_max_ps(a, _mm_mul_ps(a, minus_one));
}

bool simd_rect_intersect4f(M128 a, M128 b)
{
        M128 r1 = _mm_shuffle_ps(a, b, _MM_SHUFFLE(1, 0, 1, 0));
        M128 r2 = _mm_shuffle_ps(b, a, _MM_SHUFFLE(3, 2, 3, 2));
        M128I res = _mm_castps_si128(_mm_cmple_ps(r1, r2));

        return _mm_movemask_ps(_mm_castsi128_ps(res)) == 0xF;
}

void
simd_rotate_rect(const glm::vec2& pos, const glm::vec2& size, float degrees,
                 float4& xout, float4& yout)
{
        const M128 s = simd_scalar4f(std::sin(glm::radians(degrees)));
        const M128 c = simd_scalar4f(std::cos(glm::radians(degrees)));
        const M128 xvec = simd_create4f(0, size.x, size.x, 0);
        const M128 yvec = simd_create4f(0, 0, size.y, size.y);

        xout = simd_dump4f(simd_add4f(
                simd_sub4f(simd_mul4f(xvec, c), simd_mul4f(yvec, s)),
                simd_scalar4f(pos.x)));
        yout = simd_dump4f(simd_add4f(
                simd_add4f(simd_mul4f(xvec, s), simd_mul4f(yvec, c)),
                simd_scalar4f(pos.y)));
}

M128 simd_flip4f(M128 a)
{
        return _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));
}
