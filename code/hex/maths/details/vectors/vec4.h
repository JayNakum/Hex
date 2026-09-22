#pragma once

#include <cmath>
#include <stdexcept>

template <typename T>
class vec4
{
public:
    union
    {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        struct { T s, t, p, q; };
    };

public:
    vec4()
        : x(0), y(0) {}
    vec4(T scalar)
        : x(scalar), y(scalar), z(scalar), w(scalar) {}
    vec4(const vec4<T>& v)
        : x(v.x), y(v.y), z(v.z), w(v.w) {}
    vec4(T _x, T _y, T _z, T _w)
        : x(_x), y(_y), z(_z), w(_w) {}
    template<typename U>
    vec4(U scalar)
        : x(static_cast<T>(scalar)), y(static_cast<T>(scalar)), z(static_cast<T>(scalar)), w(static_cast<T>(scalar)) {}

    template<typename U>
    vec4(const vec4<U> v)
        : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w)) {}

    template<typename A, typename B, typename C, typename D>
    vec4(A _x, B _y, C _z, D _w)
        : x(static_cast<T>(_x)), y(static_cast<T>(_y)), z(static_cast<T>(_z)), w(static_cast<T>(_w)) {}

    static const size_t length = 4;

public:
    inline const T& operator[](size_t i) const
    {
        if (i >= vec4::length)
            throw std::out_of_range("vec4");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }
    inline T& operator[](size_t i)
    {
        if (i >= vec4::length)
            throw std::out_of_range("vec4");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    inline vec4<T>& operator=(const vec4<T>& v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = v.w;
        return *this;
    }
    template<typename U>
    inline vec4<T>& operator=(const vec4<U>& v)
    {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(v.z);
        this->w = static_cast<T>(v.w);
        return *this;
    }

    template<typename U>
    inline vec4<T>& operator+=(U scalar)
    {
        return (*this = *this + static_cast<T>(scalar));
    }
    inline vec4<T>& operator+=(const vec4<T>& v)
    {
        return (*this = *this + v);
    }

    template<typename U>
    inline vec4<T>& operator-=(U scalar)
    {
        return (*this = *this - static_cast<T>(scalar));
    }
    inline vec4<T>& operator-=(const vec4<T>& v)
    {
        return (*this = *this - v);
    }

    template<typename U>
    inline vec4<T>& operator*=(U scalar)
    {
        return (*this = *this * static_cast<T>(scalar));
    }
    inline vec4<T>& operator*=(const vec4<T>& v)
    {
        return (*this = *this * v);
    }

    template<typename U>
    inline vec4<T>& operator/=(U scalar)
    {
        return (*this = *this / static_cast<T>(scalar));
    }
    inline vec4<T>& operator/=(const vec4<T>& v)
    {
        return (*this = *this / v);
    }

    template<typename U>
    inline vec4<T>& operator%=(U scalar)
    {
        return (*this = *this % static_cast<T>(scalar));
    }
    inline vec4<T>& operator%=(const vec4<T>& v)
    {
        return (*this = *this % v);
    }

    inline vec4<T>& operator++()
    {
        ++this->x;
        ++this->y;
        ++this->z;
        ++this->w;
        return *this;
    }
    inline vec4<T>& operator--()
    {
        --this->x;
        --this->y;
        --this->z;
        --this->w;
        return *this;
    }

    inline vec4<T> operator++(int)
    {
        vec4<T> result(*this);
        ++*this;
        return result;
    }
    inline vec4<T> operator--(int)
    {
        vec4<T> result(*this);
        --*this;
        return result;
    }
};

template <typename T>
inline vec4<T> operator+(const vec4<T>& v)
{
    return v;
}
template <typename T>
inline vec4<T> operator-(const vec4<T>& v)
{
    return vec4<T>(-v.x, -v.y, -v.z, -v.w);
}

template <typename T>
inline vec4<T> operator+(const vec4<T>& v, T scalar)
{
    return vec4<T>(
        v.x + scalar,
        v.y + scalar,
        v.z + scalar,
        v.w + scalar
    );
}
template <typename T>
inline vec4<T> operator+(T scalar, const vec4<T>& v)
{
    return vec4<T>(
        scalar + v.x,
        scalar + v.y,
        scalar + v.z,
        scalar + v.w
    );
}
template <typename T>
inline vec4<T> operator+(const vec4<T>& v1, const vec4<T>& v2)
{
    return vec4<T>(
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z,
        v1.w + v2.w
    );
}

template <typename T>
inline vec4<T> operator-(const vec4<T>& v, T scalar)
{
    return vec4<T>(
        v.x - scalar,
        v.y - scalar,
        v.z - scalar,
        v.w - scalar
    );
}
template <typename T>
inline vec4<T> operator-(T scalar, const vec4<T>& v)
{
    return vec4<T>(
        scalar - v.x,
        scalar - v.y,
        scalar - v.z,
        scalar - v.w
    );
}
template <typename T>
inline vec4<T> operator-(const vec4<T>& v1, const vec4<T>& v2)
{
    return vec4<T>(
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z,
        v1.w - v2.w
    );
}

template <typename T>
inline vec4<T> operator*(const vec4<T>& v, T scalar)
{
    return vec4<T>(
        v.x * scalar,
        v.y * scalar,
        v.z * scalar,
        v.w * scalar
    );
}
template <typename T>
inline vec4<T> operator*(T scalar, const vec4<T>& v)
{
    return vec4<T>(
        scalar * v.x,
        scalar * v.y,
        scalar * v.z,
        scalar * v.w
    );
}
template <typename T>
inline vec4<T> operator*(const vec4<T>& v1, const vec4<T>& v2)
{
    return vec4<T>(
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z,
        v1.w * v2.w
    );
}

template <typename T>
inline vec4<T> operator/(const vec4<T>& v, T scalar)
{
    return vec4<T>(
        v.x / scalar,
        v.y / scalar,
        v.z / scalar,
        v.w / scalar
    );
}
template <typename T>
inline vec4<T> operator/(T scalar, const vec4<T>& v)
{
    return vec4<T>(
        scalar / v.x,
        scalar / v.y,
        scalar / v.z,
        scalar / v.w
    );
}
template <typename T>
inline vec4<T> operator/(const vec4<T>& v1, const vec4<T>& v2)
{
    return vec4<T>(
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z,
        v1.w / v2.w
    );
}

template <typename T>
inline vec4<T> operator%(const vec4<T>& v, T scalar)
{
    return vec4<T>(
        v.x % scalar,
        v.y % scalar,
        v.z % scalar,
        v.w % scalar
    );
}
template <typename T>
inline vec4<T> operator%(T scalar, const vec4<T>& v)
{
    return vec4<T>(
        scalar % v.x,
        scalar % v.y,
        scalar % v.z,
        scalar % v.w
    );
}
template <typename T>
inline vec4<T> operator%(const vec4<T>& v1, const vec4<T>& v2)
{
    return vec4<T>(
        v1.x % v2.x,
        v1.y % v2.y,
        v1.z % v2.z,
        v1.w % v2.w
    );
}

template <typename T>
inline bool operator==(const vec4<T>& v1, const vec4<T>& v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
}

template <typename T>
inline bool operator!=(const vec4<T>& v1, const vec4<T>& v2)
{
    return !(v1 == v2);
}

template <typename T>
inline T magnitude(const vec4<T>& v)
{
    return std::sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z +
        v.w * v.w
    );
}

template<typename T>
inline T dot(const vec4<T>& v1, const vec4<T>& v2)
{
    return (v1.x * v2.x +
            v1.y * v2.y +
            v1.z * v2.z +
            v1.w * v2.w);
}
