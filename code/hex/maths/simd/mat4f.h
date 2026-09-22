#pragma once

#include "../details/matrices/mat4x4.h"

#ifdef ENABLE_SIMD
#include "_m128_operations.h"
#endif

#ifndef ENABLE_SIMD
inline vec4<float> operator*(const vec4<float> v_row, const mat4x4<float>& m)
{
    return vec4<float>(
        v_row.x * m[0][0] + v_row.y * m[1][0] + v_row.z * m[2][0] + v_row.w * m[3][0],
        v_row.x * m[0][1] + v_row.y * m[1][1] + v_row.z * m[2][1] + v_row.w * m[3][1],
        v_row.x * m[0][2] + v_row.y * m[1][2] + v_row.z * m[2][2] + v_row.w * m[3][2],
        v_row.x * m[0][3] + v_row.y * m[1][3] + v_row.z * m[2][3] + v_row.w * m[3][3]
    );
}

inline vec4<float> operator*(const mat4x4<float>& m, const vec4<float> v_col)
{
    return vec4<float>(
        m[0][0] * v_col.x + m[0][1] * v_col.y + m[0][2] * v_col.z + m[0][3] * v_col.w,
        m[1][0] * v_col.x + m[1][1] * v_col.y + m[1][2] * v_col.z + m[1][3] * v_col.w,
        m[2][0] * v_col.x + m[2][1] * v_col.y + m[2][2] * v_col.z + m[2][3] * v_col.w,
        m[3][0] * v_col.x + m[3][1] * v_col.y + m[3][2] * v_col.z + m[3][3] * v_col.w
    );
}

inline mat4x4<float> operator*(const mat4x4<float>& m1, const mat4x4<float>& m2)
{
    const float a00 = m1[0][0];
    const float a01 = m1[0][1];
    const float a02 = m1[0][2];
    const float a03 = m1[0][3];
    const float a10 = m1[1][0];
    const float a11 = m1[1][1];
    const float a12 = m1[1][2];
    const float a13 = m1[1][3];
    const float a20 = m1[2][0];
    const float a21 = m1[2][1];
    const float a22 = m1[2][2];
    const float a23 = m1[2][3];
    const float a30 = m1[3][0];
    const float a31 = m1[3][1];
    const float a32 = m1[3][2];
    const float a33 = m1[3][3];

    const float b00 = m2[0][0];
    const float b01 = m2[0][1];
    const float b02 = m2[0][2];
    const float b03 = m2[0][3];
    const float b10 = m2[1][0];
    const float b11 = m2[1][1];
    const float b12 = m2[1][2];
    const float b13 = m2[1][3];
    const float b20 = m2[2][0];
    const float b21 = m2[2][1];
    const float b22 = m2[2][2];
    const float b23 = m2[2][3];
    const float b30 = m2[3][0];
    const float b31 = m2[3][1];
    const float b32 = m2[3][2];
    const float b33 = m2[3][3];

    mat4x4<float> result;
    result[0][0] = a00 * b00 + a01 * b10 + a02 * b20 + a03 * b30;
    result[0][1] = a00 * b01 + a01 * b11 + a02 * b21 + a03 * b31;
    result[0][2] = a00 * b02 + a01 * b12 + a02 * b22 + a03 * b32;
    result[0][3] = a00 * b03 + a01 * b13 + a02 * b23 + a03 * b33;

    result[1][0] = a10 * b00 + a11 * b10 + a12 * b20 + a13 * b30;
    result[1][1] = a10 * b01 + a11 * b11 + a12 * b21 + a13 * b31;
    result[1][2] = a10 * b02 + a11 * b12 + a12 * b22 + a13 * b32;
    result[1][3] = a10 * b03 + a11 * b13 + a12 * b23 + a13 * b33;

    result[2][0] = a20 * b00 + a21 * b10 + a22 * b20 + a23 * b30;
    result[2][1] = a20 * b01 + a21 * b11 + a22 * b21 + a23 * b31;
    result[2][2] = a20 * b02 + a21 * b12 + a22 * b22 + a23 * b32;
    result[2][3] = a20 * b03 + a21 * b13 + a22 * b23 + a23 * b33;

    result[3][0] = a30 * b00 + a31 * b10 + a32 * b20 + a33 * b30;
    result[3][1] = a30 * b01 + a31 * b11 + a32 * b21 + a33 * b31;
    result[3][2] = a30 * b02 + a31 * b12 + a32 * b22 + a33 * b32;
    result[3][3] = a30 * b03 + a31 * b13 + a32 * b23 + a33 * b33;

    return result;
}
#else

inline vec4<float> operator*(const vec4<float> v_row, const mat4x4<float>& m)
{
    __m128 row_x = SIMD_SSE::toM128(v_row.x);
    __m128 p_x = SIMD_SSE::mul(row_x, m[0].vec);

    __m128 row_y = SIMD_SSE::toM128(v_row.y);
    __m128 p_y = SIMD_SSE::mul(row_y, m[1].vec);

    __m128 s_xy = SIMD_SSE::add(p_x, p_y);

    __m128 row_z = SIMD_SSE::toM128(v_row.z);
    __m128 p_z = SIMD_SSE::mul(row_z, m[2].vec);

    __m128 row_w = SIMD_SSE::toM128(v_row.w);
    __m128 p_w = SIMD_SSE::mul(row_w, m[3].vec);

    __m128 s_zw = SIMD_SSE::add(p_z, p_w);

    vec4<float> result;
    result.vec = SIMD_SSE::add(s_xy, s_zw);

    return result;

}

inline vec4<float> operator*(const mat4x4<float>& m, const vec4<float> v_col)
{
    __m128 m_0 = SIMD_SSE::toM128(m[0][0], m[1][0], m[2][0], m[3][0]);
    __m128 col_x = SIMD_SSE::toM128(v_col.x);
    __m128 p_x = SIMD_SSE::mul(m_0, col_x);

    __m128 m_1 = SIMD_SSE::toM128(m[0][1], m[1][1], m[2][1], m[3][1]);
    __m128 col_y = SIMD_SSE::toM128(v_col.y);
    __m128 p_y = SIMD_SSE::mul(m_1, col_y);

    __m128 s_xy = SIMD_SSE::add(p_x, p_y);

    __m128 m_2 = SIMD_SSE::toM128(m[0][2], m[1][2], m[2][2], m[3][2]);
    __m128 col_z = SIMD_SSE::toM128(v_col.z);
    __m128 p_z = SIMD_SSE::mul(m_2, col_z);

    __m128 m_3 = SIMD_SSE::toM128(m[0][3], m[1][3], m[2][3], m[3][3]);
    __m128 col_w = SIMD_SSE::toM128(v_col.w);
    __m128 p_w = SIMD_SSE::mul(m_3, col_w);

    __m128 s_zw = SIMD_SSE::add(p_z, p_w);

    vec4<float> result;
    result.vec = SIMD_SSE::add(s_xy, s_zw);
    return result;

}

inline mat4x4<float> operator*(const mat4x4<float>& m1, const mat4x4<float>& m2)
{
    const float a00 = m1[0][0];
    const float a01 = m1[0][1];
    const float a02 = m1[0][2];
    const float a03 = m1[0][3];
    const float a10 = m1[1][0];
    const float a11 = m1[1][1];
    const float a12 = m1[1][2];
    const float a13 = m1[1][3];
    const float a20 = m1[2][0];
    const float a21 = m1[2][1];
    const float a22 = m1[2][2];
    const float a23 = m1[2][3];
    const float a30 = m1[3][0];
    const float a31 = m1[3][1];
    const float a32 = m1[3][2];
    const float a33 = m1[3][3];

    mat4x4<float> result;

    __m128 _a00 = SIMD_SSE::toM128(a00);
    __m128 _a01 = SIMD_SSE::toM128(a01);
    __m128 _a02 = SIMD_SSE::toM128(a02);
    __m128 _a03 = SIMD_SSE::toM128(a03);

    __m128 p_00 = SIMD_SSE::mul(_a00, m2[0].vec);
    __m128 p_01 = SIMD_SSE::mul(_a01, m2[1].vec);
    __m128 p_02 = SIMD_SSE::mul(_a02, m2[2].vec);
    __m128 p_03 = SIMD_SSE::mul(_a03, m2[3].vec);

    __m128 s00 = SIMD_SSE::add(p_00, p_01);
    __m128 s01 = SIMD_SSE::add(p_02, p_03);

    result[0].vec = SIMD_SSE::add(s00, s01);

    __m128 _a10 = SIMD_SSE::toM128(a10);
    __m128 _a11 = SIMD_SSE::toM128(a11);
    __m128 _a12 = SIMD_SSE::toM128(a12);
    __m128 _a13 = SIMD_SSE::toM128(a13);

    __m128 p_10 = SIMD_SSE::mul(_a10, m2[0].vec);
    __m128 p_11 = SIMD_SSE::mul(_a11, m2[1].vec);
    __m128 p_12 = SIMD_SSE::mul(_a12, m2[2].vec);
    __m128 p_13 = SIMD_SSE::mul(_a13, m2[3].vec);

    __m128 s10 = SIMD_SSE::add(p_10, p_11);
    __m128 s11 = SIMD_SSE::add(p_12, p_13);

    result[1].vec = SIMD_SSE::add(s10, s11);

    __m128 _a20 = SIMD_SSE::toM128(a20);
    __m128 _a21 = SIMD_SSE::toM128(a21);
    __m128 _a22 = SIMD_SSE::toM128(a22);
    __m128 _a23 = SIMD_SSE::toM128(a23);

    __m128 p_20 = SIMD_SSE::mul(_a20, m2[0].vec);
    __m128 p_21 = SIMD_SSE::mul(_a21, m2[1].vec);
    __m128 p_22 = SIMD_SSE::mul(_a22, m2[2].vec);
    __m128 p_23 = SIMD_SSE::mul(_a23, m2[3].vec);

    __m128 s20 = SIMD_SSE::add(p_20, p_21);
    __m128 s21 = SIMD_SSE::add(p_22, p_23);

    result[2].vec = SIMD_SSE::add(s20, s21);

    __m128 _a30 = SIMD_SSE::toM128(a30);
    __m128 _a31 = SIMD_SSE::toM128(a31);
    __m128 _a32 = SIMD_SSE::toM128(a32);
    __m128 _a33 = SIMD_SSE::toM128(a33);

    __m128 p_30 = SIMD_SSE::mul(_a30, m2[0].vec);
    __m128 p_31 = SIMD_SSE::mul(_a31, m2[1].vec);
    __m128 p_32 = SIMD_SSE::mul(_a32, m2[2].vec);
    __m128 p_33 = SIMD_SSE::mul(_a33, m2[3].vec);

    __m128 s30 = SIMD_SSE::add(p_30, p_31);
    __m128 s31 = SIMD_SSE::add(p_32, p_33);

    result[3].vec = SIMD_SSE::add(s30, s31);

    return result;

}
#endif
