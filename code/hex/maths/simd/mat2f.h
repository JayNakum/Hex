#pragma once

#include "../details/matrices/mat2x2.h"

#ifdef ENABLE_SIMD
#include "_m128_operations.h"
#endif

#ifndef ENABLE_SIMD
inline vec2<float> operator*(const vec2<float> v_row, const mat2x2<float> m)
{
    return vec2<float>(
        v_row.x * m[0][0] + v_row.y * m[1][0],
        v_row.x * m[0][1] + v_row.y * m[1][1]
    );
}

inline vec2<float> operator*(const mat2x2<float> m, const vec2<float> v_col)
{
    return vec2<float>(
        m[0][0] * v_col.x + m[0][1] * v_col.y,
        m[1][0] * v_col.x + m[1][1] * v_col.y
    );
}

inline mat2x2<float> operator*(const mat2x2<float>& m1, const mat2x2<float>& m2)
{
    return mat2x2<float>(
        m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0],
        m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1],

        m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0],
        m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1]
    );
}
#else
inline vec2<float> operator*(const vec2<float> v_row, const mat2x2<float> m)
{
    __m128 row_x = SIMD_SSE::toM128(v_row.x);
    __m128 row_y = SIMD_SSE::toM128(v_row.y);

    __m128 p_x = SIMD_SSE::mul(row_x, m[0].vec);
    __m128 p_y = SIMD_SSE::mul(row_y, m[1].vec);

    vec2<float> result;
    result.vec = SIMD_SSE::add(p_x, p_y);

    return result;

}

inline vec2<float> operator*(const mat2x2<float> m, const vec2<float> v_col)
{
    __m128 col_x = SIMD_SSE::toM128(v_col.x);
    __m128 col_y = SIMD_SSE::toM128(v_col.y);

    __m128 m_0 = SIMD_SSE::toM128(m[0][0], m[1][0], 0.0f, 0.0f);
    __m128 m_1 = SIMD_SSE::toM128(m[0][1], m[1][1], 0.0f, 0.0f);

    __m128 p_x = SIMD_SSE::mul(m_0, col_x);
    __m128 p_y = SIMD_SSE::mul(m_1, col_y);

    vec2<float> result;
    result.vec = SIMD_SSE::add(p_x, p_y);
    return result;

}

inline mat2x2<float> operator*(const mat2x2<float>& m1, const mat2x2<float>& m2)
{
    __m128 m1_0 = SIMD_SSE::toM128(m1[0][0], m1[0][0], m1[1][0], m1[1][0]);
    __m128 m2_0 = SIMD_SSE::toM128(m2[0][0], m2[0][1], m2[0][0], m2[0][1]);

    __m128 m1_1 = SIMD_SSE::toM128(m1[0][1], m1[0][1], m1[1][1], m1[1][1]);
    __m128 m2_1 = SIMD_SSE::toM128(m2[1][0], m2[1][1], m2[1][0], m2[1][1]);

    __m128 p_x = SIMD_SSE::mul(m1_0, m2_0);
    __m128 p_y = SIMD_SSE::mul(m1_1, m2_1);

    union { __m128 res;    float f[4]; };
    res = SIMD_SSE::add(p_x, p_y);

    return mat2x2<float>(f[0], f[1], f[2], f[3]);

}
#endif
