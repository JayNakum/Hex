#pragma once

#include "../../vectors.h"

template <typename T>
class mat3x3
{
    using vec = vec3<T>;
public:
    mat3x3()
        : value{ vec(1, 0, 0), vec(0, 1, 0), vec(0, 0, 1)} {}
    mat3x3(const mat3x3<T>& m)
        : value{ vec(m[0]), vec(m[1]), vec(m[2])} {}
    mat3x3(T scalar)
        : value{ vec(scalar, 0, 0), vec(0, scalar, 0), vec(0, 0, scalar)} {}
    mat3x3(
        const T& _x0, const T& _y0, const T& _z0,
        const T& _x1, const T& _y1, const T& _z1,
        const T& _x2, const T& _y2, const T& _z2
    )
        : value{ vec(_x0, _y0, _z0), vec(_x1, _y1, _z1), vec(_x2, _y2, _z2)} {}
    mat3x3(const vec3<T>& v0, const vec3<T>& v1, const vec3<T>& v2)
        : value{ v0, v1, v2 } {}
    template<typename U>
    mat3x3(const mat3x3<U>& m)
        : value{ vec(m[0]), vec(m[1]), vec(m[2])} {}
    template<typename A, typename B, typename C,
            typename D, typename E, typename F,
            typename G, typename H, typename I>
    mat3x3(const A& _x0, const B& _y0, const C& _z0,
            const D& _x1, const E& _y1, const F& _z1,
            const G& _x2, const H& _y2, const I& _z2)
        : value{ vec(_x0, _y0, _z0), vec(_x1, _y1, _z1), vec(_x2, _y2, _z2) } {}
    template<typename A, typename B, typename C>
    mat3x3(const vec3<A>& v0, const vec3<B>& v1, const vec3<C>& v2)
        : value{ vec(v0), vec(v1), vec(v2)} {}
    static const size_t length = 3;

public:
    inline vec& operator[](size_t i)
    {
        if (i >= mat3x3::length)
            throw std::out_of_range("mat3x3");
        return this->value[i];
    }
    inline vec const& operator[](size_t i) const
    {
        if (i >= mat3x3::length)
            throw std::out_of_range("mat3x3");
        return this->value[i];
    }

    template<typename U>
    inline mat3x3<T>& operator=(const mat3x3<U>& m)
    {
        this->value[0] = m[0];
        this->value[1] = m[1];
        this->value[2] = m[2];
        return *this;
    }

    template<typename U>
    inline mat3x3<T>& operator+=(U scalar)
    {
        this->value[0] += scalar;
        this->value[1] += scalar;
        this->value[2] += scalar;
        return *this;
    }
    template<typename U>
    inline mat3x3<T>& operator+=(const mat3x3<U>& m)
    {
        this->value[0] += m[0];
        this->value[1] += m[1];
        this->value[2] += m[2];
        return *this;
    }

    template<typename U>
    inline mat3x3<T>& operator-=(U scalar)
    {
        this->value[0] -= scalar;
        this->value[1] -= scalar;
        this->value[2] -= scalar;
        return *this;
    }
    template<typename U>
    inline mat3x3<T>& operator-=(const mat3x3<U>& m)
    {
        this->value[0] -= m[0];
        this->value[1] -= m[1];
        this->value[2] -= m[2];
        return *this;
    }

    template<typename U>
    inline mat3x3<T>& operator*=(U scalar)
    {
        this->value[0] *= scalar;
        this->value[1] *= scalar;
        this->value[2] *= scalar;
        return *this;
    }
    template<typename U>
    inline mat3x3<T>& operator*=(const mat3x3<U>& m)
    {
        return (*this = *this * m);
    }

    template<typename U>
    inline mat3x3<T>& operator/=(U scalar)
    {
        this->value[0] /= scalar;
        this->value[1] /= scalar;
        this->value[2] /= scalar;
        return *this;
    }
    template<typename U>
    inline mat3x3<T>& operator/=(const mat3x3<U>& m)
    {
        return (*this *= inverse(m));
    }

    inline mat3x3<T>& operator++()
    {
        ++this->value[0];
        ++this->value[1];
        ++this->value[2];
        return *this;
    }
    inline mat3x3<T> operator++(int)
    {
        mat3x3<T> result(*this);
        ++*this;
        return result;
    }
    inline mat3x3<T>& operator--()
    {
        --this->value[0];
        --this->value[1];
        --this->value[2];
        return *this;
    }
    inline mat3x3<T> operator--(int)
    {
        mat3x3<T> result(*this);
        --*this;
        return result;
    }

private:
    vec value[3];
};

template <typename T>
inline mat3x3<T> operator+(const mat3x3<T>& m)
{
    return m;
}
template <typename T>
inline mat3x3<T> operator-(const mat3x3<T>& m)
{
    return mat3x3<T>(
        -m[0],
        -m[1],
        -m[2]
    );
}

template <typename T>
inline mat3x3<T> operator+(const mat3x3<T> m, T scalar)
{
    return mat3x3<T>(
        m[0] + scalar,
        m[1] + scalar,
        m[2] + scalar
    );
}
template <typename T>
inline mat3x3<T> operator+(T scalar, const mat3x3<T> m)
{
    return mat3x3<T>(
        scalar + m[0],
        scalar + m[1],
        scalar + m[2]
    );
}
template <typename T>
inline mat3x3<T> operator+(const mat3x3<T> m1, const mat3x3<T> m2)
{
    return mat3x3<T>(
        m1[0] + m2[0],
        m1[1] + m2[1],
        m1[2] + m2[2]
    );
}

template <typename T>
inline mat3x3<T> operator-(const mat3x3<T> m, T scalar)
{
    return mat3x3<T>(
        m[0] - scalar,
        m[1] - scalar,
        m[2] - scalar
    );
}
template <typename T>
inline mat3x3<T> operator-(T scalar, const mat3x3<T> m)
{
    return mat3x3<T>(
        scalar - m[0],
        scalar - m[1],
        scalar - m[2]
    );
}
template <typename T>
inline mat3x3<T> operator-(const mat3x3<T> m1, const mat3x3<T> m2)
{
    return mat3x3<T>(
        m1[0] - m2[0],
        m1[1] - m2[1],
        m1[2] - m2[2]
    );
}

template <typename T>
inline mat3x3<T> operator*(const mat3x3<T> m, T scalar)
{
    return mat3x3<T>(
        m[0] * scalar,
        m[1] * scalar,
        m[2] * scalar
    );
}
template <typename T>
inline mat3x3<T> operator*(T scalar, const mat3x3<T> m)
{
    return mat3x3<T>(
        scalar * m[0],
        scalar * m[1],
        scalar * m[2]
    );
}
template <typename T>
inline vec3<T> operator*(const vec3<T> v_row, const mat3x3<T> m)
{
    return vec3<T>(
        v_row.x * m[0][0] + v_row.y * m[1][0] + v_row.z * m[2][0],
        v_row.x * m[0][1] + v_row.y * m[1][1] + v_row.z * m[2][1],
        v_row.x * m[0][2] + v_row.y * m[1][2] + v_row.z * m[2][2]
    );
}
template <typename T>
inline vec3<T> operator*(const mat3x3<T> m, const vec3<T> v_col)
{
    return vec3<T>(
        m[0][0] * v_col.x + m[0][1] * v_col.y + m[0][2] * v_col.z,
        m[1][0] * v_col.x + m[1][1] * v_col.y + m[1][2] * v_col.z,
        m[2][0] * v_col.x + m[2][1] * v_col.y + m[2][2] * v_col.z
    );
}
template <typename T>
inline mat3x3<T> operator*(const mat3x3<T>& m1, const mat3x3<T>& m2)
{
    const T a00 = m1[0][0];
    const T a01 = m1[0][1];
    const T a02 = m1[0][2];
    const T a10 = m1[1][0];
    const T a11 = m1[1][1];
    const T a12 = m1[1][2];
    const T a20 = m1[2][0];
    const T a21 = m1[2][1];
    const T a22 = m1[2][2];

    const T b00 = m2[0][0];
    const T b01 = m2[0][1];
    const T b02 = m2[0][2];
    const T b10 = m2[1][0];
    const T b11 = m2[1][1];
    const T b12 = m2[1][2];
    const T b20 = m2[2][0];
    const T b21 = m2[2][1];
    const T b22 = m2[2][2];

    mat3x3<T> result;
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

template <typename T>
inline mat3x3<T> operator/(const mat3x3<T> m, T scalar)
{
    return mat3x3<T>(
        m[0] / scalar,
        m[1] / scalar,
        m[2] / scalar
    );
}
template <typename T>
inline mat3x3<T> operator/(T scalar, const mat3x3<T> m)
{
    return mat3x3<T>(
        scalar / m[0],
        scalar / m[1],
        scalar / m[2]
    );
}
template <typename T>
inline vec3<T> operator/(const vec3<T> v_row, const mat3x3<T> m)
{
    return v_row / inverse(m);
}
template <typename T>
inline vec3<T> operator/(const mat3x3<T> m, const vec3<T> v_col)
{
    return inverse(m) / v_col;
}
template <typename T>
inline mat3x3<T> operator/(const mat3x3<T>& m1, const mat3x3<T>& m2)
{
    mat3x3<T> m1_cpy(m1);
    return m1_cpy /= m2;
}

template <typename T>
inline bool operator==(const mat3x3<T>& m1, const mat3x3<T>& m2)
{
    return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]);
}
template <typename T>
inline bool operator!=(const mat3x3<T>& m1, const mat3x3<T>& m2)
{
    return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]);
}

template <typename T>
inline mat3x3<T> transpose(const mat3x3<T>& m)
{
    return mat3x3<T>(
        m[0][0],
        m[1][0],
        m[2][0],
        m[0][1],
        m[1][1],
        m[2][1],
        m[0][2],
        m[1][2],
        m[2][2]
    );
}

template <typename T>
inline T determinant(const mat3x3<T>& m)
{
    return (
        + m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
        - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
        + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0])
        );
}

template <typename T>
inline mat3x3<T> adjoint(const mat3x3<T>& m)
{
    return mat3x3<T>(
        + (m[1][1] * m[2][2] - m[1][2] * m[2][1]),
        - (m[0][1] * m[2][2] - m[0][2] * m[2][1]),
        + (m[0][1] * m[1][2] - m[0][2] * m[1][1]),
        - (m[1][0] * m[2][2] - m[1][2] * m[2][0]),
        + (m[0][0] * m[2][2] - m[0][2] * m[2][0]),
        - (m[0][0] * m[1][2] - m[0][2] * m[1][0]),
        + (m[1][0] * m[2][1] - m[1][1] * m[2][0]),
        - (m[0][0] * m[2][1] - m[0][1] * m[2][0]),
        + (m[0][0] * m[1][1] - m[0][1] * m[1][0])
    );
}

template <typename T>
inline mat3x3<T> inverse(const mat3x3<T>& m)
{
    T oneOverDeterminant = static_cast<T>(1) / determinant<T>(m);

    return oneOverDeterminant * adjoint<T>(m);
}

template <typename T>
inline mat3x3<T> rotation3D(const vec3<T> axis, const T theta)
{
    vec3<T> nAxis = axis / magnitude<T>(axis);

    vec3<T> p = {
        (nAxis.x * nAxis.x) * (1 - std::cos(static_cast<float>(theta))) + std::cos(static_cast<float>(theta)),
        (nAxis.x * nAxis.y) * (1 - std::cos(static_cast<float>(theta))) + (nAxis.z * std::sin(static_cast<float>(theta))),
        (nAxis.x * nAxis.z) * (1 - std::cos(static_cast<float>(theta))) - (nAxis.y * std::sin(static_cast<float>(theta)))
    };

    vec3<T> q = {
        (nAxis.x * nAxis.y) * (1 - std::cos(static_cast<float>(theta))) - (nAxis.z * std::sin(static_cast<float>(theta))),
        (nAxis.y * nAxis.y) * (1 - std::cos(static_cast<float>(theta))) + std::cos(static_cast<float>(theta)),
        (nAxis.y * nAxis.z) * (1 - std::cos(static_cast<float>(theta))) + (nAxis.x * std::sin(static_cast<float>(theta)))
    };

    vec3<T> r = {
        (nAxis.x * nAxis.z) * (1 - std::cos(static_cast<float>(theta))) + (nAxis.y * std::sin(static_cast<float>(theta))),
        (nAxis.y * nAxis.z) * (1 - std::cos(static_cast<float>(theta))) - (nAxis.x * std::sin(static_cast<float>(theta))),
        (nAxis.z * nAxis.z) * (1 - std::cos(static_cast<float>(theta))) + std::cos(static_cast<float>(theta))
    };

    return mat3x3<T>(p, q, r);
}
