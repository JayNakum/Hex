#pragma once

#include <cmath>
#include <stdexcept>

template <typename T>
class vec3
{
public:
    union
    {
        struct { T x, y, z; };
        struct { T r, g, b; };
        struct { T s, t, p; };
    };

public:
    vec3()
        : x(0), y(0), z(0) {}
    vec3(T scalar)
        : x(scalar), y(scalar), z(scalar) {}
    vec3(const vec3<T>& v)
        : x(v.x), y(v.y), z(v.z) {}
    vec3(T _x, T _y, T _z)
        : x(_x), y(_y), z(_z) {}
    template<typename U>
    vec3(U scalar)
        : x(static_cast<T>(scalar)), y(static_cast<T>(scalar)), z(static_cast<T>(scalar)) {}

    template<typename U>
    vec3(const vec3<U> v)
        : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)) {}

    template<typename A, typename B, typename C>
    vec3(A _x, B _y, C _z)
        : x(static_cast<T>(_x)), y(static_cast<T>(_y)), z(static_cast<T>(_z)) {}

    static const size_t length = 3;

public:
    inline const T& operator[](size_t i) const
    {
        if (i >= vec3::length)
            throw std::out_of_range("vec3");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }
    inline T& operator[](size_t i)
    {
        if (i >= vec3::length)
            throw std::out_of_range("vec3");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    inline vec3<T>& operator=(const vec3<T>& v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        return *this;
    }
    template<typename U>
    inline vec3<T>& operator=(const vec3<U>& v)
    {
        this->x = static_cast<T>(v.x);
        this->y = static_cast<T>(v.y);
        this->z = static_cast<T>(v.z);
        return *this;
    }

    template<typename U>
    inline vec3<T>& operator+=(U scalar)
    {
        return (*this = *this + static_cast<T>(scalar));
    }
    inline vec3<T>& operator+=(const vec3<T>& v)
    {
        return (*this = *this + v);
    }

    template<typename U>
    inline vec3<T>& operator-=(U scalar)
    {
        return (*this = *this - static_cast<T>(scalar));
    }
    inline vec3<T>& operator-=(const vec3<T>& v)
    {
        return (*this = *this - v);
    }

    template<typename U>
    inline vec3<T>& operator*=(U scalar)
    {
        return (*this = *this * static_cast<T>(scalar));
    }
    inline vec3<T>& operator*=(const vec3<T>& v)
    {
        return (*this = *this * v);
    }

    template<typename U>
    inline vec3<T>& operator/=(U scalar)
    {
        return (*this = *this / static_cast<T>(scalar));
    }
    inline vec3<T>& operator/=(const vec3<T>& v)
    {
        return (*this = *this / v);
    }

    template<typename U>
    inline vec3<T>& operator%=(U scalar)
    {
        return (*this = *this % static_cast<T>(scalar));
    }
    inline vec3<T>& operator%=(const vec3<T>& v)
    {
        return (*this = *this % v);
    }

    inline vec3<T>& operator++()
    {
        ++this->x;
        ++this->y;
        ++this->z;
        return *this;
    }
    inline vec3<T> operator++(int)
    {
        vec3<T> result(*this);
        ++*this;
        return result;
    }

    inline vec3<T>& operator--()
    {
        --this->x;
        --this->y;
        --this->z;
        return *this;
    }
    inline vec3<T> operator--(int)
    {
        vec3<T> result(*this);
        --*this;
        return result;
    }
};

template <typename T>
inline vec3<T> operator+(const vec3<T>& v)
{
    return v;
}
template <typename T>
inline vec3<T> operator-(const vec3<T>& v)
{
    return vec3<T>(-v.x, -v.y, -v.z);
}

template <typename T>
inline vec3<T> operator+(const vec3<T>& v, T scalar)
{
    return vec3<T>(
        v.x + scalar,
        v.y + scalar,
        v.z + scalar
    );
}
template <typename T>
inline vec3<T> operator+(T scalar, const vec3<T>& v)
{
    return vec3<T>(
        scalar + v.x,
        scalar + v.y,
        scalar + v.z
    );
}
template <typename T>
inline vec3<T> operator+(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z
    );
}

template <typename T>
inline vec3<T> operator-(const vec3<T>& v, T scalar)
{
    return vec3<T>(
        v.x - scalar,
        v.y - scalar,
        v.z - scalar
    );
}
template <typename T>
inline vec3<T> operator-(T scalar, const vec3<T>& v)
{
    return vec3<T>(
        scalar - v.x,
        scalar - v.y,
        scalar - v.z
    );
}
template <typename T>
inline vec3<T> operator-(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z
    );
}

template <typename T>
inline vec3<T> operator*(const vec3<T>& v, T scalar)
{
    return vec3<T>(
        v.x * scalar,
        v.y * scalar,
        v.z * scalar
    );
}
template <typename T>
inline vec3<T> operator*(T scalar, const vec3<T>& v)
{
    return vec3<T>(
        scalar * v.x,
        scalar * v.y,
        scalar * v.z
    );
}
template <typename T>
inline vec3<T> operator*(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z
    );
}

template <typename T>
inline vec3<T> operator/(const vec3<T>& v, T scalar)
{
    return vec3<T>(
        v.x / scalar,
        v.y / scalar,
        v.z / scalar
    );
}
template <typename T>
inline vec3<T> operator/(T scalar, const vec3<T>& v)
{
    return vec3<T>(
        scalar / v.x,
        scalar / v.y,
        scalar / v.z
    );
}
template <typename T>
inline vec3<T> operator/(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z
    );
}

template <typename T>
inline vec3<T> operator%(const vec3<T>& v, T scalar)
{
    return vec3<T>(
        v.x % scalar,
        v.y % scalar,
        v.z % scalar
    );
}
template <typename T>
inline vec3<T> operator%(T scalar, const vec3<T>& v)
{
    return vec3<T>(
        scalar % v.x,
        scalar % v.y,
        scalar % v.z
    );
}
template <typename T>
inline vec3<T> operator%(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(
        v1.x % v2.x,
        v1.y % v2.y,
        v1.z % v2.z
    );
}

template <typename T>
inline bool operator==(const vec3<T>& v1, const vec3<T>& v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

template <typename T>
inline bool operator!=(const vec3<T>& v1, const vec3<T>& v2)
{
    return !(v1 == v2);
}

template <typename T>
inline T magnitude(const vec3<T>& v)
{
    return std::sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );
}

template<typename T>
inline T dot(const vec3<T>& v1, const vec3<T>& v2)
{
    return (v1.x * v2.x +
        v1.y * v2.y +
        v1.z * v2.z);
}

template<typename T>
inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}
