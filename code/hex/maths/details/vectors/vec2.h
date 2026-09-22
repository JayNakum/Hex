#pragma once

#include <cmath>
#include <stdexcept>

template <typename T>
class vec2
{
public:
    union
    {
        struct { T x, y; };
        struct { T r, g; };
        struct { T s, t; };
    };

public:
    vec2()
        : x(0), y(0) {}
    vec2(T scalar)
        : x(scalar), y(scalar) {}
    vec2(const vec2<T>& v)
        : x(v.x), y(v.y) {}
    vec2(T _x, T _y)
        : x(_x), y(_y) {}
    template<typename U>
    vec2(U scalar)
        : x(static_cast<T>(scalar)), y(static_cast<T>(scalar)) {}

    template<typename U>
    vec2(const vec2<U> v)
        : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

    template<typename A, typename B>
    vec2(A _x, B _y)
        : x(static_cast<T>(_x)), y(static_cast<T>(_y)) {}

    static const size_t length = 2;

public:
    inline T const& operator[](size_t i) const
    {
        if (i >= vec2::length)
            throw std::out_of_range("vec2");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }
    inline T& operator[](size_t i)
    {
        if (i >= vec2::length)
            throw std::out_of_range("vec2");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    inline vec2<T>& operator=(const vec2<T>& v)
    {
        this->x = v.x;
        this->y = v.y;
        return *this;
    }
    template<typename U>
    inline vec2<T>& operator=(const vec2<U>& v)
    {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        return *this;
    }

    template<typename U>
    inline vec2<T>& operator+=(U scalar)
    {
        return (*this = *this + scalar);
    }
    inline vec2<T>& operator+=(const vec2<T>& v)
    {
        return (*this = *this + v);
    }

    template<typename U>
    inline vec2<T>& operator-=(U scalar)
    {
        return (*this = *this - scalar);
    }
    inline vec2<T>& operator-=(const vec2<T>& v)
    {
        return (*this = *this - v);
    }

    template<typename U>
    inline vec2<T>& operator*=(U scalar)
    {
        return (*this = *this * scalar);
    }
    inline vec2<T>& operator*=(const vec2<T>& v)
    {
        return (*this = *this * v);
    }

    template<typename U>
    inline vec2<T>& operator/=(U scalar)
    {
        return (*this = *this / scalar);
    }
    inline vec2<T>& operator/=(const vec2<T>& v)
    {
        return (*this = *this / v);
    }

    template<typename U>
    inline vec2<T>& operator%=(U scalar)
    {
        return (*this = *this % scalar);
    }
    inline vec2<T>& operator%=(const vec2<T>& v)
    {
        return (*this = *this % v);
    }

    inline vec2<T>& operator++()
    {
        ++this->x;
        ++this->y;
        return *this;
    }
    inline vec2<T> operator++(int)
    {
        vec2<T> result(*this);
        ++*this;
        return result;
    }

    inline vec2<T>& operator--()
    {
        --this->x;
        --this->y;
        return *this;
    }
    inline vec2<T> operator--(int)
    {
        vec2<T> result(*this);
        --*this;
        return result;
    }
};

template <typename T>
inline vec2<T> operator+(const vec2<T>& v)
{
    return v;
}
template <typename T>
inline vec2<T> operator-(const vec2<T>& v)
{
    return vec2<T>(-v.x, -v.y);
}

template <typename T, typename U>
inline vec2<T> operator+(const vec2<T>& v, U scalar)
{
    return vec2<T>(
        v.x + static_cast<T>(scalar),
        v.y + static_cast<T>(scalar)
    );
}
template <typename T, typename U>
inline vec2<T> operator+(U scalar, const vec2<T>& v)
{
    return vec2<T>(
        static_cast<T>(scalar) + v.x,
        static_cast<T>(scalar) + v.y
    );
}
template <typename T>
inline vec2<T> operator+(const vec2<T>& v1, const vec2<T>& v2)
{
    return vec2<T>(
        v1.x + v2.x,
        v1.y + v2.y
    );
}

template <typename T, typename U>
inline vec2<T> operator-(const vec2<T>& v, U scalar)
{
    return vec2<T>(
        v.x - static_cast<T>(scalar),
        v.y - static_cast<T>(scalar)
    );
}
template <typename T, typename U>
inline vec2<T> operator-(U scalar, const vec2<T>& v)
{
    return vec2<T>(
        static_cast<T>(scalar) - v.x,
        static_cast<T>(scalar) - v.y
    );
}
template <typename T>
inline vec2<T> operator-(const vec2<T>& v1, const vec2<T>& v2)
{
    return vec2<T>(
        v1.x - v2.x,
        v1.y - v2.y
    );
}

template <typename T, typename U>
inline vec2<T> operator*(const vec2<T>& v, U scalar)
{
    return vec2<T>(
        v.x * static_cast<T>(scalar),
        v.y * static_cast<T>(scalar)
    );
}
template <typename T, typename U>
inline vec2<T> operator*(U scalar, const vec2<T>& v)
{
    return vec2<T>(
        static_cast<T>(scalar) * v.x,
        static_cast<T>(scalar) * v.y
    );
}
template <typename T>
inline vec2<T> operator*(const vec2<T>& v1, const vec2<T>& v2)
{
    return vec2<T>(
        v1.x * v2.x,
        v1.y * v2.y
    );
}

template <typename T, typename U>
inline vec2<T> operator/(const vec2<T>& v, U scalar)
{
    return vec2<T>(
        v.x / static_cast<T>(scalar),
        v.y / static_cast<T>(scalar)
    );
}
template <typename T, typename U>
inline vec2<T> operator/(U scalar, const vec2<T>& v)
{
    return vec2<T>(
        static_cast<T>(scalar) / v.x,
        static_cast<T>(scalar) / v.y
    );
}
template <typename T>
inline vec2<T> operator/(const vec2<T>& v1, const vec2<T>& v2)
{
    return vec2<T>(
        v1.x / v2.x,
        v1.y / v2.y
    );
}

template <typename T, typename U>
inline vec2<T> operator%(const vec2<T>& v, U scalar)
{
    return vec2<T>(
        v.x % static_cast<T>(scalar),
        v.y % static_cast<T>(scalar)
    );
}
template <typename T, typename U>
inline vec2<T> operator%(U scalar, const vec2<T>& v)
{
    return vec2<T>(
        static_cast<T>(scalar) % v.x,
        static_cast<T>(scalar) % v.y
    );
}
template <typename T>
inline vec2<T> operator%(const vec2<T>& v1, const vec2<T>& v2)
{
    return vec2<T>(
        v1.x % v2.x,
        v1.y % v2.y
    );
}

template <typename T>
inline bool operator==(const vec2<T>& v1, const vec2<T>& v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y);
}

template <typename T>
inline bool operator!=(const vec2<T>& v1, const vec2<T>& v2)
{
    return !(v1 == v2);
}

template <typename T>
inline T magnitude(const vec2<T>& v)
{
    return std::sqrt(
        v.x * v.x +
        v.y * v.y
    );
}

template <typename T>
inline T dot(const vec2<T>& v1, const vec2<T>& v2)
{
    return (v1.x * v2.x +
        v1.y * v2.y);
}
