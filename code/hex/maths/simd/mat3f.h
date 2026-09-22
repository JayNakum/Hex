#pragma once

#include "../details/matrices/mat3x3.h"

#ifdef ENABLE_SIMD
#include "_m128_operations.h"
#endif

#ifndef ENABLE_SIMD

inline vec3<float> operator*(const vec3<float> v_row, const mat3x3<float> m)
{
    return vec3<float>(
        v_row.x * m[0][0] + v_row.y * m[1][0] + v_row.z * m[2][0],
        v_row.x * m[0][1] + v_row.y * m[1][1] + v_row.z * m[2][1],
        v_row.x * m[0][2] + v_row.y * m[1][2] + v_row.z * m[2][2]
    );
}

inline vec3<float> operator*(const mat3x3<float> m, const vec3<float> v_col)
{
    return vec3<float>(
        m[0][0] * v_col.x + m[0][1] * v_col.y + m[0][2] * v_col.z,
        m[1][0] * v_col.x + m[1][1] * v_col.y + m[1][2] * v_col.z,
        m[2][0] * v_col.x + m[2][1] * v_col.y + m[2][2] * v_col.z
    );
}

inline mat3x3<float> operator*(const mat3x3<float>& m1, const mat3x3<float>& m2)
{
    const float a00 = m1[0][0];
    const float a01 = m1[0][1];
    const float a02 = m1[0][2];
    const float a10 = m1[1][0];
    const float a11 = m1[1][1];
    const float a12 = m1[1][2];
    const float a20 = m1[2][0];
    const float a21 = m1[2][1];
    const float a22 = m1[2][2];

    const float b00 = m2[0][0];
    const float b01 = m2[0][1];
    const float b02 = m2[0][2];
    const float b10 = m2[1][0];
    const float b11 = m2[1][1];
    const float b12 = m2[1][2];
    const float b20 = m2[2][0];
    const float b21 = m2[2][1];
    const float b22 = m2[2][2];

    mat3x3<float> result;
    result[0][0] = a00 * b00 + a01 * b10 + a02 * b20;
    result[0][1] = a00 * b01 + a01 * b11 + a02 * b21;
    result[0][2] = a00 * b02 + a01 * b12 + a02 * b22;

    result[1][0] = a10 * b00 + a11 * b10 + a12 * b20;
    result[1][1] = a10 * b01 + a11 * b11 + a12 * b21;
    result[1][2] = a10 * b02 + a11 * b12 + a12 * b22;

    result[2][0] = a20 * b00 + a21 * b10 + a22 * b20;
    result[2][1] = a20 * b01 + a21 * b11 + a22 * b21;
    result[2][2] = a20 * b02 + a21 * b12 + a22 * b22;
    return result;
}

#else

inline vec3<float> operator*(const vec3<float> v_row, const mat3x3<float> m)
{
    __m128 row_x = SIMD_SSE::toM128(v_row.x);
    __m128 p_x = SIMD_SSE::mul(row_x, m[0].vec);

    __m128 row_y = SIMD_SSE::toM128(v_row.y);
    __m128 p_y = SIMD_SSE::mul(row_y, m[1].vec);

    __m128 row_z = SIMD_SSE::toM128(v_row.z);
    __m128 p_z = SIMD_SSE::mul(row_z, m[2].vec);

    __m128 s_xy = SIMD_SSE::add(p_x, p_y);

    vec3<float> result;
    result.vec = SIMD_SSE::add(s_xy, p_z);
    return result;

}

inline vec3<float> operator*(const mat3x3<float> m, const vec3<float> v_col)
{
    __m128 m_0 = SIMD_SSE::toM128(m[0][0], m[1][0], m[2][0], 0.0f);
    __m128 col_x = SIMD_SSE::toM128(v_col.x);
    __m128 p_x = SIMD_SSE::mul(m_0, col_x);

    __m128 m_1 = SIMD_SSE::toM128(m[0][1], m[1][1], m[2][1], 0.0f);
    __m128 col_y = SIMD_SSE::toM128(v_col.y);
    __m128 p_y = SIMD_SSE::mul(m_1, col_y);

    __m128 m_2 = SIMD_SSE::toM128(m[0][2], m[1][2], m[2][2], 0.0f);
    __m128 col_z = SIMD_SSE::toM128(v_col.z);
    __m128 p_z = SIMD_SSE::mul(m_2, col_z);

    __m128 s_xy = SIMD_SSE::add(p_x, p_y);

    vec3<float> result;
    result.vec = SIMD_SSE::add(s_xy, p_z);
    return result;

}

inline mat3x3<float> operator*(const mat3x3<float>& m1, const mat3x3<float>& m2)
{
    const float a00 = m1[0][0];
    const float a01 = m1[0][1];
    const float a02 = m1[0][2];
    const float a10 = m1[1][0];
    const float a11 = m1[1][1];
    const float a12 = m1[1][2];
    const float a20 = m1[2][0];
    const float a21 = m1[2][1];
    const float a22 = m1[2][2];

    mat3x3<float> result;

    __m128 _a00 = SIMD_SSE::toM128(a00);
    __m128 p_00 = SIMD_SSE::mul(_a00, m2[0].vec);

    __m128 _a01 = SIMD_SSE::toM128(a01);
    __m128 p_01 = SIMD_SSE::mul(_a01, m2[1].vec);

    __m128 s0 = SIMD_SSE::add(p_00, p_01);

    __m128 _a02 = SIMD_SSE::toM128(a02);
    __m128 p_02 = SIMD_SSE::mul(_a02, m2[2].vec);

    result[0].vec = SIMD_SSE::add(s0, p_02);

    __m128 _a10 = SIMD_SSE::toM128(a10);
    __m128 p_10 = SIMD_SSE::mul(_a10, m2[0].vec);

    __m128 _a11 = SIMD_SSE::toM128(a11);
    __m128 p_11 = SIMD_SSE::mul(_a11, m2[1].vec);

    __m128 s1 = SIMD_SSE::add(p_10, p_11);

    __m128 _a12 = SIMD_SSE::toM128(a12);
    __m128 p_12 = SIMD_SSE::mul(_a12, m2[2].vec);

    result[1].vec = SIMD_SSE::add(s1, p_12);

    __m128 _a20 = SIMD_SSE::toM128(a20);
    __m128 p_20 = SIMD_SSE::mul(_a20, m2[0].vec);

    __m128 _a21 = SIMD_SSE::toM128(a21);
    __m128 p_21 = SIMD_SSE::mul(_a21, m2[1].vec);

    __m128 s2 = SIMD_SSE::add(p_20, p_21);

    __m128 _a22 = SIMD_SSE::toM128(a22);
    __m128 p_22 = SIMD_SSE::mul(_a22, m2[2].vec);

    result[2].vec = SIMD_SSE::add(s2, p_22);

    return result;

}
#endif
