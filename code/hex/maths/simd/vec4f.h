#pragma once

#include "../details/vectors/vec4.h"

#ifdef ENABLE_SIMD
#include "_m128_operations.h"
#endif

template <>
class vec4<float>
{
public:
    union
    {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
        struct { float s, t, p, q; };
#ifdef ENABLE_SIMD
        __m128 vec;
#endif
    };

public:
    vec4()
        : x(0), y(0), z(0), w(0) {}
    vec4(float scalar)
        : x(scalar), y(scalar), z(scalar), w(scalar) {}
    vec4(const vec4<float>& v)
        : x(v.x), y(v.y), z(v.z), w(v.w) {}
    vec4(float _x, float _y, float _z, float _w)
        : x(_x), y(_y), z(_z), w(_w) {}
    template<typename U>
    vec4(U scalar)
        : x(static_cast<float>(scalar)), y(static_cast<float>(scalar)), z(static_cast<float>(scalar)), w(static_cast<float>(scalar)) {}

    template<typename U>
    vec4(const vec4<U> v)
        : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)), z(static_cast<float>(v.z)), w(static_cast<float>(v.w)) {}

    template<typename A, typename B, typename C, typename D>
    vec4(A _x, B _y, C _z, D _w)
        : x(static_cast<float>(_x)), y(static_cast<float>(_y)), z(static_cast<float>(_z)), w(static_cast<float>(_w)) {}

    static const size_t length = 4;

public:
    inline const float& operator[](size_t i) const
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
    inline float& operator[](size_t i)
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
#ifndef ENABLE_SIMD
    inline vec4<float>& operator=(const vec4<float>& v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
        this->w = v.w;
        return *this;
    }
    template<typename U>
    inline vec4<float>& operator=(const vec4<U>& v)
    {
        this->x = static_cast<float>(v.x);
        this->y = static_cast<float>(v.y);
        this->z = static_cast<float>(v.z);
        this->w = static_cast<float>(v.w);
        return *this;
    }
#else
    inline vec4<float>& operator=(const vec4<float>& v)
    {
        this->vec = v.vec;
        return *this;
    }
    template<typename U>
    inline vec4<float>& operator=(const vec4<U>& v)
    {
        this->vec = SIMD_SSE::toM128(v.x, v.y, v.z);
        return *this;
    }
#endif

    template<typename U>
    inline vec4<float>& operator+=(U scalar)
    {
        return (*this = *this + static_cast<float>(scalar));
    }
    inline vec4<float>& operator+=(const vec4<float>& v)
    {
        return (*this = *this + v);
    }

    template<typename U>
    inline vec4<float>& operator-=(U scalar)
    {
        return (*this = *this - static_cast<float>(scalar));
    }
    inline vec4<float>& operator-=(const vec4<float>& v)
    {
        return (*this = *this - v);
    }

    template<typename U>
    inline vec4<float>& operator*=(U scalar)
    {
        return (*this = *this * static_cast<float>(scalar));
    }
    inline vec4<float>& operator*=(const vec4<float>& v)
    {
        return (*this = *this * v);
    }

    template<typename U>
    inline vec4<float>& operator/=(U scalar)
    {
        return (*this = *this / static_cast<float>(scalar));
    }
    inline vec4<float>& operator/=(const vec4<float>& v)
    {
        return (*this = *this / v);
    }

#ifndef ENABLE_SIMD
    inline vec4<float>& operator++()
    {
        ++this->x;
        ++this->y;
        ++this->z;
        ++this->w;
        return *this;
    }
    inline vec4<float>& operator--()
    {
        --this->x;
        --this->y;
        --this->z;
        --this->w;
        return *this;
    }
#else
    inline vec4<float>& operator++()
    {
        this->vec = SIMD_SSE::increment(this->vec);
        return *this;
    }

    inline vec4<float>& operator--()
    {
        this->vec = SIMD_SSE::decrement(this->vec);
        return *this;
    }
#endif
    inline vec4<float> operator++(int)
    {
        vec4<float> result(*this);
        ++*this;
        return result;
    }
    inline vec4<float> operator--(int)
    {
        vec4<float> result(*this);
        --*this;
        return result;
    }
};

inline vec4<float> operator+(const vec4<float>& v)
{
    return v;
}
inline vec4<float> operator-(const vec4<float>& v)
{
    return vec4<float>(-v.x, -v.y, -v.z, -v.w);
}

#ifndef ENABLE_SIMD
inline vec4<float> operator+(const vec4<float>& v, float scalar)
{
    return vec4<float>(
        v.x + static_cast<float>(scalar),
        v.y + static_cast<float>(scalar),
        v.z + static_cast<float>(scalar),
        v.w + static_cast<float>(scalar)
    );
}
inline vec4<float> operator+(float scalar, const vec4<float>& v)
{
    return vec4<float>(
        static_cast<float>(scalar) + v.x,
        static_cast<float>(scalar) + v.y,
        static_cast<float>(scalar) + v.z,
        static_cast<float>(scalar) + v.w
    );
}
inline vec4<float> operator+(const vec4<float>& v1, const vec4<float>& v2)
{
    return vec4<float>(
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z,
        v1.w + v2.w
    );
}

inline vec4<float> operator-(const vec4<float>& v, float scalar)
{
    return vec4<float>(
        v.x - static_cast<float>(scalar),
        v.y - static_cast<float>(scalar),
        v.z - static_cast<float>(scalar),
        v.w - static_cast<float>(scalar)
    );
}
inline vec4<float> operator-(float scalar, const vec4<float>& v)
{
    return vec4<float>(
        static_cast<float>(scalar) - v.x,
        static_cast<float>(scalar) - v.y,
        static_cast<float>(scalar) - v.z,
        static_cast<float>(scalar) - v.w
    );
}
inline vec4<float> operator-(const vec4<float>& v1, const vec4<float>& v2)
{
    return vec4<float>(
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z,
        v1.w - v2.w
    );
}

inline vec4<float> operator*(const vec4<float>& v, float scalar)
{
    return vec4<float>(
        v.x * static_cast<float>(scalar),
        v.y * static_cast<float>(scalar),
        v.z * static_cast<float>(scalar),
        v.w * static_cast<float>(scalar)
    );
}
inline vec4<float> operator*(float scalar, const vec4<float>& v)
{
    return vec4<float>(
        static_cast<float>(scalar) * v.x,
        static_cast<float>(scalar) * v.y,
        static_cast<float>(scalar) * v.z,
        static_cast<float>(scalar) * v.w
    );
}
inline vec4<float> operator*(const vec4<float>& v1, const vec4<float>& v2)
{
    return vec4<float>(
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z,
        v1.w * v2.w
    );
}

inline vec4<float> operator/(const vec4<float>& v, float scalar)
{
    return vec4<float>(
        v.x / static_cast<float>(scalar),
        v.y / static_cast<float>(scalar),
        v.z / static_cast<float>(scalar),
        v.w / static_cast<float>(scalar)
    );
}
inline vec4<float> operator/(float scalar, const vec4<float>& v)
{
    return vec4<float>(
        static_cast<float>(scalar) / v.x,
        static_cast<float>(scalar) / v.y,
        static_cast<float>(scalar) / v.z,
        static_cast<float>(scalar) / v.w
    );
}
inline vec4<float> operator/(const vec4<float>& v1, const vec4<float>& v2)
{
    return vec4<float>(
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z,
        v1.w / v2.w
    );
}

template<typename T>
inline T dot(const vec4<float>& v1, const vec4<float>& v2)
{
    return (v1.x * v2.x +
        v1.y * v2.y +
        v1.z * v2.z +
        v1.w * v2.w);
}
#else
inline vec4<float> operator+(const vec4<float>& v, float scalar)
{
    vec4<float> result;
    result.vec = SIMD_SSE::add(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec4<float> operator+(float scalar, const vec4<float>& v)
{
    vec4<float> result;
    result.vec = SIMD_SSE::add(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec4<float> operator+(const vec4<float>& v1, const vec4<float>& v2)
{
    vec4<float> result;
    result.vec = SIMD_SSE::add(v1.vec, v2.vec);
    return result;
}

inline vec4<float> operator-(const vec4<float>& v, float scalar)
{
    vec4<float> result;
    result.vec = SIMD_SSE::sub(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec4<float> operator-(float scalar, const vec4<float>& v)
{
    vec4<float> result;
    result.vec = SIMD_SSE::sub(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec4<float> operator-(const vec4<float>& v1, const vec4<float>& v2)
{
    vec4<float> result;
    result.vec = SIMD_SSE::sub(v1.vec, v2.vec);
    return result;
}

inline vec4<float> operator*(const vec4<float>& v, float scalar)
{
    vec4<float> result;
    result.vec = SIMD_SSE::mul(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec4<float> operator*(float scalar, const vec4<float>& v)
{
    vec4<float> result;
    result.vec = SIMD_SSE::mul(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec4<float> operator*(const vec4<float>& v1, const vec4<float>& v2)
{
    vec4<float> result;
    result.vec = SIMD_SSE::mul(v1.vec, v2.vec);
    return result;
}

inline vec4<float> operator/(const vec4<float>& v, float scalar)
{
    vec4<float> result;
    result.vec = SIMD_SSE::div(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec4<float> operator/(float scalar, const vec4<float>& v)
{
    vec4<float> result;
    result.vec = SIMD_SSE::div(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec4<float> operator/(const vec4<float>& v1, const vec4<float>& v2)
{
    vec4<float> result;
    result.vec = SIMD_SSE::div(v1.vec, v2.vec);
    return result;
}

inline float dot(const vec4<float>& v1, const vec4<float>& v2)
{
    vec4<float> result;
    result.vec = SIMD_SSE::mul(v1.vec, v2.vec);
    return result.x + result.y + result.z + result.w;
}
#endif

template <typename T>
inline bool operator==(const vec4<float>& v1, const vec4<float>& v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
}

template <typename T>
inline bool operator!=(const vec4<float>& v1, const vec4<float>& v2)
{
    return !(v1 == v2);
}

inline float magnitude(const vec4<float>& v)
{
    return std::sqrt(dot(v, v));
}
