#pragma once

#include "../vectors/vec2.h"

#include <cmath>

template <typename T>
class mat2x2
{
    using vec = vec2<T>;
public:
    mat2x2()
        : value{vec(1, 0), vec(0, 1)} {}
    mat2x2(const mat2x2<T>& m)
        : value{vec(m[0]), vec(m[1])} {}
    mat2x2(T scalar)
        : value{vec(scalar, 0), vec(0, scalar)} {}
    mat2x2(const T& _x0, const T& _y0, const T& _x1, const T& _y1)
        : value{vec(_x0, _y0), vec(_x1, _y1)} {}
    mat2x2(const vec2<T>& v0, const vec2<T>& v1)
        : value{v0, v1} {}
    template<typename U>
    mat2x2(const mat2x2<U>& m)
        : value{ vec(m[0]), vec(m[1]) } {}
    template<typename A, typename B, typename C, typename D>
    mat2x2(const A& _x0, const B& _y0, const C& _x1, const D& _y1)
        : value{ vec(_x0, _y0), vec(_x1, _y1) } {}
    template<typename A, typename B>
    mat2x2(const vec2<A>& v0, const vec2<B>& v1)
        : value{ vec(v0), vec(v1) } {}
    static const size_t length = 2;

public:
    inline vec& operator[](size_t i)
    {
        if (i >= mat2x2::length)
            throw std::out_of_range("mat2x2");
        return this->value[i];
    }
    inline vec const& operator[](size_t i) const
    {
        if (i >= mat2x2::length)
            throw std::out_of_range("mat2x2");
        return this->value[i];
    }

    template<typename U>
    inline mat2x2<T>& operator=(const mat2x2<U>& m)
    {
        this->value[0] = m[0];
        this->value[1] = m[1];
        return *this;
    }

    template<typename U>
    inline mat2x2<T>& operator+=(U scalar)
    {
        this->value[0] += scalar;
        this->value[1] += scalar;
        return *this;
    }
    template<typename U>
    inline mat2x2<T>& operator+=(const mat2x2<U>& m)
    {
        this->value[0] += m[0];
        this->value[1] += m[1];
        return *this;
    }

    template<typename U>
    inline mat2x2<T>& operator-=(U scalar)
    {
        this->value[0] -= scalar;
        this->value[1] -= scalar;
        return *this;
    }
    template<typename U>
    inline mat2x2<T>& operator-=(const mat2x2<U>& m)
    {
        this->value[0] -= m[0];
        this->value[1] -= m[1];
        return *this;
    }

    template<typename U>
    inline mat2x2<T>& operator*=(U scalar)
    {
        this->value[0] *= scalar;
        this->value[1] *= scalar;
        return *this;
    }
    template<typename U>
    inline mat2x2<T>& operator*=(const mat2x2<U>& m)
    {
        return (*this = *this * m);
    }

    template<typename U>
    inline mat2x2<T>& operator/=(U scalar)
    {
        this->value[0] /= scalar;
        this->value[1] /= scalar;
        return *this;
    }
    template<typename U>
    inline mat2x2<T>& operator/=(const mat2x2<U>& m)
    {
        return (*this *= inverse(m));
    }

    inline mat2x2<T>& operator++()
    {
        ++this->value[0];
        ++this->value[1];
        return *this;
    }
    inline mat2x2<T> operator++(int)
    {
        mat2x2<T> result(*this);
        ++*this;
        return result;
    }
    inline mat2x2<T>& operator--()
    {
        --this->value[0];
        --this->value[1];
        return *this;
    }
    inline mat2x2<T> operator--(int)
    {
        mat2x2<T> result(*this);
        --*this;
        return result;
    }

private:
    vec value[2];
};

template <typename T>
inline mat2x2<T> operator+(const mat2x2<T>& m)
{
    return m;
}
template <typename T>
inline mat2x2<T> operator-(const mat2x2<T>& m)
{
    return mat2x2<T>(
        -m[0],
        -m[1]
    );
}

template <typename T>
inline mat2x2<T> operator+(const mat2x2<T> m, T scalar)
{
    return mat2x2<T>(
        m[0] + scalar,
        m[1] + scalar
    );
}
template <typename T>
inline mat2x2<T> operator+(T scalar, const mat2x2<T> m)
{
    return mat2x2<T>(
        scalar + m[0],
        scalar + m[1]
    );
}
template <typename T>
inline mat2x2<T> operator+(const mat2x2<T> m1, const mat2x2<T> m2)
{
    return mat2x2<T>(
        m1[0] + m2[0],
        m1[1] + m2[1]
    );
}

template <typename T>
inline mat2x2<T> operator-(const mat2x2<T> m, T scalar)
{
    return mat2x2<T>(
        m[0] - scalar,
        m[1] - scalar
    );
}
template <typename T>
inline mat2x2<T> operator-(T scalar, const mat2x2<T> m)
{
    return mat2x2<T>(
        scalar - m[0],
        scalar - m[1]
    );
}
template <typename T>
inline mat2x2<T> operator-(const mat2x2<T> m1, const mat2x2<T> m2)
{
    return mat2x2<T>(
        m1[0] - m2[0],
        m1[1] - m2[1]
    );
}

template <typename T>
inline mat2x2<T> operator*(const mat2x2<T> m, T scalar)
{
    return mat2x2<T>(
        m[0] * scalar,
        m[1] * scalar
    );
}
template <typename T>
inline mat2x2<T> operator*(T scalar, const mat2x2<T> m)
{
    return mat2x2<T>(
        scalar * m[0],
        scalar * m[1]
    );
}
template <typename T>
inline vec2<T> operator*(const vec2<T> v_row, const mat2x2<T> m)
{
    return vec2<T>(
        v_row.x * m[0][0] + v_row.y * m[1][0],
        v_row.x * m[0][1] + v_row.y * m[1][1]
    );
}
template <typename T>
inline vec2<T> operator*(const mat2x2<T> m, const vec2<T> v_col)
{
    return vec2<T>(
        m[0][0] * v_col.x + m[0][1] * v_col.y,
        m[1][0] * v_col.x + m[1][1] * v_col.y
    );
}
template <typename T>
inline mat2x2<T> operator*(const mat2x2<T>& m1, const mat2x2<T>& m2)
{
    return mat2x2<T>(
        m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0],
        m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1],

        m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0],
        m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1]
    );
}

template <typename T>
inline mat2x2<T> operator/(const mat2x2<T> m, T scalar)
{
    return mat2x2<T>(
        m[0] / scalar,
        m[1] / scalar
    );
}
template <typename T>
inline mat2x2<T> operator/(T scalar, const mat2x2<T> m)
{
    return mat2x2<T>(
        scalar / m[0],
        scalar / m[1]
    );
}
template <typename T>
inline vec2<T> operator/(const vec2<T> v_row, const mat2x2<T> m)
{
    return v_row / inverse(m);
}
template <typename T>
inline vec2<T> operator/(const mat2x2<T> m, const vec2<T> v_col)
{
    return inverse(m) / v_col;
}
template <typename T>
inline mat2x2<T> operator/(const mat2x2<T>& m1, const mat2x2<T>& m2)
{
    mat2x2<T> m1_cpy(m1);
    return m1_cpy /= m2;
}

template <typename T>
inline bool operator==(const mat2x2<T>& m1, const mat2x2<T>& m2)
{
    return (m1[0] == m2[0]) && (m1[1] == m2[1]);
}
template <typename T>
inline bool operator!=(const mat2x2<T>& m1, const mat2x2<T>& m2)
{
    return (m1[0] != m2[0]) || (m1[1] != m2[1]);
}

template <typename T>
inline mat2x2<T> transpose(const mat2x2<T>& m)
{
    return mat2x2<T>(
        m[0][0],
        m[1][0],
        m[0][1],
        m[1][1]
    );
}

template <typename T>
inline T determinant(const mat2x2<T>& m)
{
    return (+m[0][0] * m[1][1] - m[0][1] * m[1][0]);
}

template <typename T>
inline mat2x2<T> adjoint(const mat2x2<T>& m)
{
    return mat2x2<T>(
        + m[1][1],
        - m[0][1],
        - m[1][0],
        + m[0][0]
    );
}

template <typename T>
inline mat2x2<T> inverse(const mat2x2<T>& m)
{
    T oneOverDeterminant = static_cast<T>(1) / determinant<T>(m);
    return oneOverDeterminant * adjoint<T>(m);
}

template <typename T>
inline mat2x2<T> rotation2D(const T theta)
{
    return mat2x2<T>(
        std::cos(theta), -std::sin(theta),
        std::sin(theta), std::cos(theta)
    );
}
