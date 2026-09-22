#pragma once

#include "../details/vectors/vec3.h"

#ifdef ENABLE_SIMD
#include "_m128_operations.h"
#endif

template <>
class vec3<float>
{
public:
    union
    {
        struct { float x, y, z; };
        struct { float r, g, b; };
        struct { float s, t, p; };
#ifdef ENABLE_SIMD
        __m128 vec;
#endif
    };

public:
    vec3()
        : x(0), y(0), z(0) {}
    vec3(float scalar)
        : x(scalar), y(scalar), z(scalar) {}
    vec3(const vec3<float>& v)
        : x(v.x), y(v.y), z(v.z) {}
    vec3(float _x, float _y, float _z)
        : x(_x), y(_y), z(_z) {}
    template<typename U>
    vec3(U scalar)
        : x(static_cast<float>(scalar)), y(static_cast<float>(scalar)), z(static_cast<float>(scalar)) {}

    template<typename U>
    vec3(const vec3<U> v)
        : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)), z(static_cast<float>(v.z)) {}

    template<typename A, typename B, typename C>
    vec3(A _x, B _y, C _z)
        : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(static_cast<float>(_z)) {}

    static const size_t length = 3;

public:
    inline const float& operator[](size_t i) const
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
    inline float& operator[](size_t i)
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

#ifndef ENABLE_SIMD
    inline vec3<float>& operator=(const vec3<float>& v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        return *this;
    }
    template<typename U>
    inline vec3<float>& operator=(const vec3<U>& v)
    {
        this->x = static_cast<float>(v.x);
        this->y = static_cast<float>(v.y);
        this->z = static_cast<float>(v.z);
        return *this;
    }
#else
    inline vec3<float>& operator=(const vec3<float>& v)
    {
        this->vec = v.vec;
        return *this;
    }
    template<typename U>
    inline vec3<float>& operator=(const vec3<U>& v)
    {
        this->vec = SIMD_SSE::toM128(v.x, v.y, v.z);
        return *this;
    }
#endif
    template<typename U>
    inline vec3<float>& operator+=(U scalar)
    {
        return (*this = *this + static_cast<float>(scalar));
    }
    inline vec3<float>& operator+=(const vec3<float>& v)
    {
        return (*this = *this + v);
    }

    template<typename U>
    inline vec3<float>& operator-=(U scalar)
    {
        return (*this = *this - static_cast<float>(scalar));
    }
    inline vec3<float>& operator-=(const vec3<float>& v)
    {
        return (*this = *this - v);
    }

    template<typename U>
    inline vec3<float>& operator*=(U scalar)
    {
        return (*this = *this * static_cast<float>(scalar));
    }
    inline vec3<float>& operator*=(const vec3<float>& v)
    {
        return (*this = *this * v);
    }

    template<typename U>
    inline vec3<float>& operator/=(U scalar)
    {
        return (*this = *this / static_cast<float>(scalar));
    }
    inline vec3<float>& operator/=(const vec3<float>& v)
    {
        return (*this = *this / v);
    }

#ifndef ENABLE_SIMD
    inline vec3<float>& operator++()
    {
        ++this->x;
        ++this->y;
        ++this->z;
        return *this;
    }
    inline vec3<float>& operator--()
    {
        --this->x;
        --this->y;
        --this->z;
        return *this;
    }
#else
    inline vec3<float>& operator++()
    {
        this->vec = SIMD_SSE::increment(this->vec);
        return *this;
    }

    inline vec3<float>& operator--()
    {
        this->vec = SIMD_SSE::decrement(this->vec);
        return *this;
    }
#endif
    inline vec3<float> operator++(int)
    {
        vec3<float> result(*this);
        ++*this;
        return result;
    }
    inline vec3<float> operator--(int)
    {
        vec3<float> result(*this);
        --*this;
        return result;
    }
};

template <typename T>
inline vec3<float> operator+(const vec3<float>& v)
{
    return v;
}
template <typename T>
inline vec3<float> operator-(const vec3<float>& v)
{
    return vec3<float>(-v.x, -v.y, -v.z);
}

#ifndef ENABLE_SIMD
inline vec3<float> operator+(const vec3<float>& v, float scalar)
{
    return vec3<float>(
        v.x + static_cast<float>(scalar),
        v.y + static_cast<float>(scalar),
        v.z + static_cast<float>(scalar)
    );
}
inline vec3<float> operator+(float scalar, const vec3<float>& v)
{
    return vec3<float>(
        static_cast<float>(scalar) + v.x,
        static_cast<float>(scalar) + v.y,
        static_cast<float>(scalar) + v.z
    );
}
inline vec3<float> operator+(const vec3<float>& v1, const vec3<float>& v2)
{
    return vec3<float>(
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z
    );
}

inline vec3<float> operator-(const vec3<float>& v, float scalar)
{
    return vec3<float>(
        v.x - static_cast<float>(scalar),
        v.y - static_cast<float>(scalar),
        v.z - static_cast<float>(scalar)
    );
}
inline vec3<float> operator-(float scalar, const vec3<float>& v)
{
    return vec3<float>(
        static_cast<float>(scalar) - v.x,
        static_cast<float>(scalar) - v.y,
        static_cast<float>(scalar) - v.z
    );
}
inline vec3<float> operator-(const vec3<float>& v1, const vec3<float>& v2)
{
    return vec3<float>(
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z
    );
}

inline vec3<float> operator*(const vec3<float>& v, float scalar)
{
    return vec3<float>(
        v.x * static_cast<float>(scalar),
        v.y * static_cast<float>(scalar),
        v.z * static_cast<float>(scalar)
    );
}
inline vec3<float> operator*(float scalar, const vec3<float>& v)
{
    return vec3<float>(
        static_cast<float>(scalar) * v.x,
        static_cast<float>(scalar) * v.y,
        static_cast<float>(scalar) * v.z
    );
}
inline vec3<float> operator*(const vec3<float>& v1, const vec3<float>& v2)
{
    return vec3<float>(
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z
    );
}

inline vec3<float> operator/(const vec3<float>& v, float scalar)
{
    return vec3<float>(
        v.x / static_cast<float>(scalar),
        v.y / static_cast<float>(scalar),
        v.z / static_cast<float>(scalar)
    );
}
inline vec3<float> operator/(float scalar, const vec3<float>& v)
{
    return vec3<float>(
        static_cast<float>(scalar) / v.x,
        static_cast<float>(scalar) / v.y,
        static_cast<float>(scalar) / v.z
    );
}
inline vec3<float> operator/(const vec3<float>& v1, const vec3<float>& v2)
{
    return vec3<float>(
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z
    );
}

template<typename T>
inline T dot(const vec3<float>& v1, const vec3<float>& v2)
{
    return (v1.x * v2.x +
        v1.y * v2.y +
        v1.z * v2.z);
}

#else
inline vec3<float> operator+(const vec3<float>& v, float scalar)
{
    vec3<float> result;
    result.vec = SIMD_SSE::add(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec3<float> operator+(float scalar, const vec3<float>& v)
{
    vec3<float> result;
    result.vec = SIMD_SSE::add(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec3<float> operator+(const vec3<float>& v1, const vec3<float>& v2)
{
    vec3<float> result;
    result.vec = SIMD_SSE::add(v1.vec, v2.vec);
    return result;
}

inline vec3<float> operator-(const vec3<float>& v, float scalar)
{
    vec3<float> result;
    result.vec = SIMD_SSE::sub(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec3<float> operator-(float scalar, const vec3<float>& v)
{
    vec3<float> result;
    result.vec = SIMD_SSE::sub(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec3<float> operator-(const vec3<float>& v1, const vec3<float>& v2)
{
    vec3<float> result;
    result.vec = SIMD_SSE::sub(v1.vec, v2.vec);
    return result;
}

inline vec3<float> operator*(const vec3<float>& v, float scalar)
{
    vec3<float> result;
    result.vec = SIMD_SSE::mul(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec3<float> operator*(float scalar, const vec3<float>& v)
{
    vec3<float> result;
    result.vec = SIMD_SSE::mul(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec3<float> operator*(const vec3<float>& v1, const vec3<float>& v2)
{
    vec3<float> result;
    result.vec = SIMD_SSE::mul(v1.vec, v2.vec);
    return result;
}

inline vec3<float> operator/(const vec3<float>& v, float scalar)
{
    vec3<float> result;
    result.vec = SIMD_SSE::div(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec3<float> operator/(float scalar, const vec3<float>& v)
{
    vec3<float> result;
    result.vec = SIMD_SSE::div(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec3<float> operator/(const vec3<float>& v1, const vec3<float>& v2)
{
    vec3<float> result;
    result.vec = SIMD_SSE::div(v1.vec, v2.vec);
    return result;
}

inline float dot(const vec3<float>& v1, const vec3<float>& v2)
{
    vec3<float> result;
    result.vec = SIMD_SSE::mul(v1.vec, v2.vec);
    return result.x + result.y + result.z;
}

#endif

template <typename T>
inline bool operator==(const vec3<float>&v1, const vec3<float>&v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

template <typename T>
inline bool operator!=(const vec3<float>&v1, const vec3<float>& v2)
{
    return !(v1 == v2);
}

inline float magnitude(const vec3<float>& v)
{
    return std::sqrt(dot(v, v));
}

inline vec3<float> cross(const vec3<float>& v1, const vec3<float>& v2)
{
    return vec3<float>(
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}
