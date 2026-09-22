#pragma once

#include "../../vectors.h"
#include "mat3x3.h"

template <typename T>
class mat4x4
{
    using vec = vec4<T>;
public:
    mat4x4()
        : value{ vec(1, 0, 0, 0), vec(0, 1, 0, 0), vec(0, 0, 1, 0), vec(0, 0, 0, 1) } {}
    mat4x4(const mat4x4<T>& m)
        : value{ vec(m[0]), vec(m[1]), vec(m[2]), vec(m[3])} {}
    mat4x4(T scalar)
        : value{ vec(scalar, 0, 0, 0), vec(0, scalar, 0, 0), vec(0, 0, scalar, 0), vec(0, 0, 0, scalar) } {}
    mat4x4(
        const T& _x0, const T& _y0, const T& _z0, const T& _w0,
        const T& _x1, const T& _y1, const T& _z1, const T& _w1,
        const T& _x2, const T& _y2, const T& _z2, const T& _w2,
        const T& _x3, const T& _y3, const T& _z3, const T& _w3)
        : value{ vec(_x0, _y0, _z0, _w0), vec(_x1, _y1, _z1, _w1), vec(_x2, _y2, _z2, _w2), vec(_x3, _y3, _z3, _w3) } {}
    mat4x4(const vec4<T>& v0, const vec4<T>& v1, const vec4<T>& v2, const vec4<T>& v3)
        : value{ v0, v1, v2, v3 } {}
    template<typename U>
    mat4x4(const mat4x4<U>& m)
        : value{ vec(m[0]), vec(m[1]), vec(m[2]), vec(m[3])} {}

    template<typename A, typename B, typename C, typename D,
        typename E, typename F, typename G, typename H,
        typename I, typename J, typename K, typename L,
        typename M, typename N, typename O, typename P>
    mat4x4(const A& _x0, const B& _y0, const C& _z0, const D& _w0,
           const E& _x1, const F& _y1, const G& _z1, const H& _w1,
           const I& _x2, const J& _y2, const K& _z2, const L& _w2,
           const M& _x3, const N& _y3, const O& _z3, const P& _w3)
        : value{ vec(_x0, _y0, _z0, _w0), vec(_x1, _y1, _z1, _w1), vec(_x2, _y2, _z2, _w2), vec(_x3, _y3, _z3, _w3) } {}

    template<typename A, typename B, typename C, typename D>
    mat4x4(const vec4<A>& v0, const vec4<B>& v1, const vec4<C>& v2, const vec4<D>& v3)
        : value{ vec(v0), vec(v1), vec(v2), vec(v3)} {}

    static const size_t length = 4;

public:
    inline vec& operator[](size_t i)
    {
        if (i >= mat4x4::length)
            throw std::out_of_range("mat4x4");
        return this->value[i];
    }
    inline vec const& operator[](size_t i) const
    {
        if (i >= mat4x4::length)
            throw std::out_of_range("mat4x4");
        return this->value[i];
    }

    template<typename U>
    inline mat4x4<T>& operator=(const mat4x4<U>& m)
    {
        this->value[0] = m[0];
        this->value[1] = m[1];
        this->value[2] = m[2];
        this->value[3] = m[3];
        return *this;
    }

    inline mat4x4& operator=(const mat4x4& m)
    {
        this->value[0] = m[0];
        this->value[1] = m[1];
        this->value[2] = m[2];
        this->value[3] = m[3];
        return *this;
    }

    template<typename U>
    inline mat4x4<T>& operator+=(U scalar)
    {
        this->value[0] += scalar;
        this->value[1] += scalar;
        this->value[2] += scalar;
        this->value[3] += scalar;
        return *this;
    }
    template<typename U>
    inline mat4x4<T>& operator+=(const mat4x4<U>& m)
    {
        this->value[0] += m[0];
        this->value[1] += m[1];
        this->value[2] += m[2];
        this->value[3] += m[3];
        return *this;
    }

    template<typename U>
    inline mat4x4<T>& operator-=(U scalar)
    {
        this->value[0] -= scalar;
        this->value[1] -= scalar;
        this->value[2] -= scalar;
        this->value[3] -= scalar;
        return *this;
    }
    template<typename U>
    inline mat4x4<T>& operator-=(const mat4x4<U>& m)
    {
        this->value[0] -= m[0];
        this->value[1] -= m[1];
        this->value[2] -= m[2];
        this->value[3] -= m[3];
        return *this;
    }

    template<typename U>
    inline mat4x4<T>& operator*=(U scalar)
    {
        this->value[0] *= scalar;
        this->value[1] *= scalar;
        this->value[2] *= scalar;
        this->value[3] *= scalar;
        return *this;
    }
    template<typename U>
    inline mat4x4<T>& operator*=(const mat4x4<U>& m)
    {
        return (*this = *this * m);
    }

    template<typename U>
    inline mat4x4<T>& operator/=(U scalar)
    {
        this->value[0] /= scalar;
        this->value[1] /= scalar;
        this->value[2] /= scalar;
        this->value[3] /= scalar;
        return *this;
    }
    template<typename U>
    inline mat4x4<T>& operator/=(const mat4x4<U>& m)
    {
        return (*this *= inverse(m));
    }

    inline mat4x4<T>& operator++()
    {
        ++this->value[0];
        ++this->value[1];
        ++this->value[2];
        ++this->value[3];
        return *this;
    }
    inline mat4x4<T> operator++(int)
    {
        mat4x4<T> result(*this);
        ++*this;
        return result;
    }
    inline mat4x4<T>& operator--()
    {
        --this->value[0];
        --this->value[1];
        --this->value[2];
        --this->value[3];
        return *this;
    }
    inline mat4x4<T> operator--(int)
    {
        mat4x4<T> result(*this);
        --*this;
        return result;
    }

    inline vec3<T> up() const
    {
        return vec3<T>(
            value[0][1],
            value[1][1],
            value[2][1]
        );
    }
    inline vec3<T> right() const
    {
        return vec3<T>(
            value[0][0],
            value[1][0],
            value[2][0]
        );
    }
    inline vec3<T> back() const
    {
        return vec3<T>(
            value[0][2],
            value[1][2],
            value[2][2]
        );
    }
    inline vec3<T> translation() const
    {
        return vec3<T>(
            value[3][0],
            value[3][1],
            value[3][2]
        );
    }
    inline vec3<T> scale() const
    {
        return vec3<T>(
            value[0][0],
            value[1][1],
            value[2][2]
        );
    }

private:
    vec value[4];
};

template <typename T>
inline mat4x4<T> operator+(const mat4x4<T>& m)
{
    return m;
}
template <typename T>
inline mat4x4<T> operator-(const mat4x4<T>& m)
{
    return mat4x4<T>(
        -m[0],
        -m[1],
        -m[2],
        -m[3]
    );
}

template <typename T>
inline mat4x4<T> operator+(const mat4x4<T>& m, T scalar)
{
    return mat4x4<T>(
        m[0] + scalar,
        m[1] + scalar,
        m[2] + scalar,
        m[3] + scalar
    );
}
template <typename T>
inline mat4x4<T> operator+(T scalar, const mat4x4<T>& m)
{
    return mat4x4<T>(
        scalar + m[0],
        scalar + m[1],
        scalar + m[2],
        scalar + m[3]
    );
}
template <typename T>
inline mat4x4<T> operator+(const mat4x4<T>& m1, const mat4x4<T>& m2)
{
    return mat4x4<T>(
        m1[0] + m2[0],
        m1[1] + m2[1],
        m1[2] + m2[2],
        m1[3] + m2[3]
    );
}

template <typename T>
inline mat4x4<T> operator-(const mat4x4<T>& m, T scalar)
{
    return mat4x4<T>(
        m[0] - scalar,
        m[1] - scalar,
        m[2] - scalar,
        m[3] - scalar
    );
}
template <typename T>
inline mat4x4<T> operator-(T scalar, const mat4x4<T>& m)
{
    return mat4x4<T>(
        scalar - m[0],
        scalar - m[1],
        scalar - m[2],
        scalar - m[3]
    );
}
template <typename T>
inline mat4x4<T> operator-(const mat4x4<T>& m1, const mat4x4<T>& m2)
{
    return mat4x4<T>(
        m1[0] - m2[0],
        m1[1] - m2[1],
        m1[2] - m2[2],
        m1[3] - m2[3]
    );
}

template <typename T>
inline mat4x4<T> operator*(const mat4x4<T>& m, T scalar)
{
    return mat4x4<T>(
        m[0] * scalar,
        m[1] * scalar,
        m[2] * scalar,
        m[3] * scalar
    );
}
template <typename T>
inline mat4x4<T> operator*(T scalar, const mat4x4<T>& m)
{
    return mat4x4<T>(
        scalar * m[0],
        scalar * m[1],
        scalar * m[2],
        scalar * m[3]
    );
}
template <typename T>
inline vec4<T> operator*(const vec4<T> v_row, const mat4x4<T>& m)
{
    return vec4<T>(
        v_row.x * m[0][0] + v_row.y * m[1][0] + v_row.z * m[2][0] + v_row.w * m[3][0],
        v_row.x * m[0][1] + v_row.y * m[1][1] + v_row.z * m[2][1] + v_row.w * m[3][1],
        v_row.x * m[0][2] + v_row.y * m[1][2] + v_row.z * m[2][2] + v_row.w * m[3][2],
        v_row.x * m[0][3] + v_row.y * m[1][3] + v_row.z * m[2][3] + v_row.w * m[3][3]
    );
}
template <typename T>
inline vec4<T> operator*(const mat4x4<T>& m, const vec4<T> v_col)
{
    return vec4<T>(
        m[0][0] * v_col.x + m[0][1] * v_col.y + m[0][2] * v_col.z + m[0][3] * v_col.w,
        m[1][0] * v_col.x + m[1][1] * v_col.y + m[1][2] * v_col.z + m[1][3] * v_col.w,
        m[2][0] * v_col.x + m[2][1] * v_col.y + m[2][2] * v_col.z + m[2][3] * v_col.w,
        m[3][0] * v_col.x + m[3][1] * v_col.y + m[3][2] * v_col.z + m[3][3] * v_col.w
    );
}
template <typename T>
inline mat4x4<T> operator*(const mat4x4<T>& m1, const mat4x4<T>& m2)
{
    const T a00 = m1[0][0];
    const T a01 = m1[0][1];
    const T a02 = m1[0][2];
    const T a03 = m1[0][3];
    const T a10 = m1[1][0];
    const T a11 = m1[1][1];
    const T a12 = m1[1][2];
    const T a13 = m1[1][3];
    const T a20 = m1[2][0];
    const T a21 = m1[2][1];
    const T a22 = m1[2][2];
    const T a23 = m1[2][3];
    const T a30 = m1[3][0];
    const T a31 = m1[3][1];
    const T a32 = m1[3][2];
    const T a33 = m1[3][3];

    const T b00 = m2[0][0];
    const T b01 = m2[0][1];
    const T b02 = m2[0][2];
    const T b03 = m2[0][3];
    const T b10 = m2[1][0];
    const T b11 = m2[1][1];
    const T b12 = m2[1][2];
    const T b13 = m2[1][3];
    const T b20 = m2[2][0];
    const T b21 = m2[2][1];
    const T b22 = m2[2][2];
    const T b23 = m2[2][3];
    const T b30 = m2[3][0];
    const T b31 = m2[3][1];
    const T b32 = m2[3][2];
    const T b33 = m2[3][3];

    mat4x4<T> result;
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

template <typename T>
inline mat4x4<T> operator/(const mat4x4<T>& m, T scalar)
{
    return mat4x4<T>(
        m[0] / scalar,
        m[1] / scalar,
        m[2] / scalar,
        m[3] / scalar
    );
}
template <typename T>
inline mat4x4<T> operator/(T scalar, const mat4x4<T>& m)
{
    return mat4x4<T>(
        scalar / m[0],
        scalar / m[1],
        scalar / m[2],
        scalar / m[3]
    );
}
template <typename T>
inline vec4<T> operator/(const vec4<T> v_row, const mat4x4<T>& m)
{
    return v_row / inverse(m);
}
template <typename T>
inline vec4<T> operator/(const mat4x4<T>& m, const vec4<T> v_col)
{
    return inverse(m) / v_col;
}
template <typename T>
inline mat4x4<T> operator/(const mat4x4<T>& m1, const mat4x4<T>& m2)
{
    mat4x4<T> m1_cpy(m1);
    return m1_cpy /= m2;
}

template <typename T>
inline bool operator==(const mat4x4<T>& m1, const mat4x4<T>& m2)
{
    return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]) && (m1[3] == m2[3]);
}
template <typename T>
inline bool operator!=(const mat4x4<T>& m1, const mat4x4<T>& m2)
{
    return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]) || (m1[3] != m2[3]);
}

template <typename T>
inline mat4x4<T> transpose(const mat4x4<T>& m)
{
    return mat4x4<T>(
        m[0][0],
        m[1][0],
        m[2][0],
        m[3][0],
        m[0][1],
        m[1][1],
        m[2][1],
        m[3][1],
        m[0][2],
        m[1][2],
        m[2][2],
        m[3][2],
        m[0][3],
        m[1][3],
        m[2][3],
        m[3][3]
    );
}

template <typename T>
inline T determinant(const mat4x4<T>& m)
{
    return
        m[0][0] * (
            m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
            m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
            )
        - m[0][1] * (
            m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0])
            )
        + m[0][2] * (
            m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
            m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0])
            )
        - m[0][3] * (
            m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
            m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) +
            m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0])
            );
}

template <typename T>
inline mat4x4<T> adjoint(const mat4x4<T>& m)
{
    const T m00 = m[0][0];
    const T m01 = m[0][1];
    const T m02 = m[0][2];
    const T m03 = m[0][3];
    const T m10 = m[1][0];
    const T m11 = m[1][1];
    const T m12 = m[1][2];
    const T m13 = m[1][3];
    const T m20 = m[2][0];
    const T m21 = m[2][1];
    const T m22 = m[2][2];
    const T m23 = m[2][3];
    const T m30 = m[3][0];
    const T m31 = m[3][1];
    const T m32 = m[3][2];
    const T m33 = m[3][3];

    const T A2323 = m22 * m33 - m23 * m32;
    const T A1323 = m21 * m33 - m23 * m31;
    const T A1223 = m21 * m32 - m22 * m31;
    const T A0323 = m20 * m33 - m23 * m30;
    const T A0223 = m20 * m32 - m22 * m30;
    const T A0123 = m20 * m31 - m21 * m30;
    const T A2313 = m12 * m33 - m13 * m32;
    const T A1313 = m11 * m33 - m13 * m31;
    const T A1213 = m11 * m32 - m12 * m31;
    const T A2312 = m12 * m23 - m13 * m22;
    const T A1312 = m11 * m23 - m13 * m21;
    const T A1212 = m11 * m22 - m12 * m21;
    const T A0313 = m10 * m33 - m13 * m30;
    const T A0213 = m10 * m32 - m12 * m30;
    const T A0312 = m10 * m23 - m13 * m20;
    const T A0212 = m10 * m22 - m12 * m20;
    const T A0113 = m10 * m31 - m11 * m30;
    const T A0112 = m10 * m21 - m11 * m20;

    return mat4x4<T>(
         (m11 * A2323 - m12 * A1323 + m13 * A1223),
        -(m01 * A2323 - m02 * A1323 + m03 * A1223),
         (m01 * A2313 - m02 * A1313 + m03 * A1213),
        -(m01 * A2312 - m02 * A1312 + m03 * A1212),

        -(m10 * A2323 - m12 * A0323 + m13 * A0223),
         (m00 * A2323 - m02 * A0323 + m03 * A0223),
        -(m00 * A2313 - m02 * A0313 + m03 * A0213),
         (m00 * A2312 - m02 * A0312 + m03 * A0212),

         (m10 * A1323 - m11 * A0323 + m13 * A0123),
        -(m00 * A1323 - m01 * A0323 + m03 * A0123),
         (m00 * A1313 - m01 * A0313 + m03 * A0113),
        -(m00 * A1312 - m01 * A0312 + m03 * A0112),

        -(m10 * A1223 - m11 * A0223 + m12 * A0123),
         (m00 * A1223 - m01 * A0223 + m02 * A0123),
        -(m00 * A1213 - m01 * A0213 + m02 * A0113),
         (m00 * A1212 - m01 * A0212 + m02 * A0112)
    );

}

template <typename T>
inline mat4x4<T> inverse(const mat4x4<T>& m)
{
    T oneOverDeterminant = static_cast<T>(1) / determinant<T>(m);
    return oneOverDeterminant * adjoint<T>(m);
}

template <typename T>
inline mat4x4<T> rotation4D(const vec3<T>& axis, const T theta)
{
    mat3x3<T> mat = rotation3D<T>(axis, theta);

    vec4<T> p = { mat[0][0], mat[0][1], mat[0][2], static_cast<T>(0) };
    vec4<T> q = { mat[1][0], mat[1][1], mat[1][2], static_cast<T>(0) };
    vec4<T> r = { mat[2][0], mat[2][1], mat[2][2], static_cast<T>(0) };
    vec4<T> s = { 0, 0, 0, 1 };

    return mat4x4<T>(p, q, r, s);
}

template <typename T>
inline mat4x4<T> lookAtLH(const vec3<T>& position, const vec3<T>& target, const vec3<T>& worldUp)
{
    vec3<T> zaxis = normalize(target - position);
    vec3<T> xaxis = normalize(cross(worldUp, zaxis));
    vec3<T> yaxis = cross(zaxis, xaxis);

    return mat4x4<T>(
        { xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, position) },
        { yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, position) },
        { zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, position) },
        { 0, 0, 0, 1 }
    );
}

template <typename T>
inline mat4x4<T> lookAtRH(const vec3<T>& position, const vec3<T>& target, const vec3<T>& worldUp)
{
    vec3<T> zaxis = normalize(position - target);
    vec3<T> xaxis = normalize(cross(normalize(worldUp), zaxis));
    vec3<T> yaxis = cross(zaxis, xaxis);

    return mat4x4<T>(
        { xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, position) },
        { yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, position) },
        { zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, position) },
        { 0, 0, 0, 1 }
    );
}

inline mat4x4<float> perspectiveProjection(float fov, float aspect, float nearVal, float farVal)
{
    float yScale = 1.0f / std::tan(fov / 2);
    float xScale = yScale / aspect;

    return mat4x4<float>(
        { xScale, 0.0f, 0.0f, 0.0f },
        { 0.0f, yScale, 0.0f, 0.0f },
        { 0.0f, 0.0f, (farVal / (farVal - nearVal)), ((-nearVal * farVal) / (farVal - nearVal)) },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    );
}

template <typename T>
inline mat4x4<T> rotate(const vec3<T>& axis, const T theta)
{
    const vec3<T> n = axis / magnitude(axis);

    const float sin_theta = static_cast<float>(std::sin(theta));
    const float cos_theta = static_cast<float>(std::cos(theta));

    vec4<T> p(
        (n.x * n.x) * (1 - cos_theta) + (cos_theta),
        (n.x * n.y) * (1 - cos_theta) + (n.z * sin_theta),
        (n.x * n.z) * (1 - cos_theta) - (n.y * sin_theta),
        0.0f
    );

    vec4<T> q(
        (n.x * n.y) * (1 - cos_theta) - (n.z * sin_theta),
        (n.y * n.y) * (1 - cos_theta) + (cos_theta),
        (n.y * n.z) * (1 - cos_theta) + (n.x * sin_theta),
        0.0f
    );

    vec4<T> r(
        (n.x * n.z) * (1 - cos_theta) + (n.y * sin_theta),
        (n.y * n.z) * (1 - cos_theta) - (n.x * sin_theta),
        (n.z * n.z) * (1 - cos_theta) + (cos_theta),
        0.0f
    );

    return mat4x4<T>(
        p,
        q,
        r,
        { 0.0f, 0.0f, 0.0f ,1.0f }
    );
}

template <typename T>
inline mat4x4<T> scale(const vec3<T>& scale)
{
    return mat4x4<T>(
        { scale.x, 0.0f, 0.0f, 0.0f },
        { 0.0f, scale.y, 0.0f, 0.0f },
        { 0.0f, 0.0f, scale.z, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    );
}

template <typename T>
inline mat4x4<T> translate(const vec3<T>& translation)
{
    return mat4x4<T>(
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { translation.x, translation.y, translation.z, 1.0f }
    );
}
