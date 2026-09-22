#pragma once

#include "../details/vectors/vec2.h"

#ifdef ENABLE_SIMD
#include "_m128_operations.h"
#endif

template <>
class vec2<float>
{
public:
    union
    {
        struct { float x, y; };
        struct { float r, g; };
        struct { float s, t; };
        struct { float u, v; };

#ifdef ENABLE_SIMD
        __m128 vec;
#endif
    };

public:
    vec2()
        : x(0), y(0) {}
    vec2(float scalar)
        : x(scalar), y(scalar) {}
    vec2(const vec2<float>& v)
        : x(v.x), y(v.y) {}
    vec2(float _x, float _y)
        : x(_x), y(_y) {}
    template<typename U>
    vec2(U scalar)
        : x(static_cast<float>(scalar)), y(static_cast<float>(scalar)) {}

    template<typename U>
    vec2(const vec2<U> v)
        : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)) {}

    template<typename A, typename B>
    vec2(A _x, B _y)
        : x(static_cast<float>(_x)), y(static_cast<float>(_y)) {}

    static const size_t length = 2;
public:
    inline float const& operator[](size_t i) const
    {
        if (i >= vec2::length)
            throw std::out_of_range("vec2f");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }
    inline float& operator[](size_t i)
    {
        if (i >= vec2::length)
            throw std::out_of_range("vec2f");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }
#ifndef ENABLE_SIMD
    inline vec2<float>& operator=(const vec2<float>& v)
    {
        this->x = v.x;
        this->y = v.y;
        return *this;
    }
    template<typename U>
    inline vec2<float>& operator=(const vec2<U>& v)
    {
        this->x = static_cast<float>(v.x);
        this->y = static_cast<float>(v.y);
        return *this;
    }
#else
    inline vec2<float>& operator=(const vec2<float>& v)
    {
        this->vec = v.vec;
        return *this;
    }
    template<typename U>
    inline vec2<float>& operator=(const vec2<U>& v)
    {
        this->vec = SIMD_SSE::toM128(v.x, v.y);
        return *this;
    }
#endif

    template<typename U>
    inline vec2<float>& operator+=(U scalar)
    {
        return (*this = *this + scalar);
    }
    inline vec2<float>& operator+=(const vec2<float>& v)
    {
        return (*this = *this + v);
    }

    template<typename U>
    inline vec2<float>& operator-=(U scalar)
    {
        return (*this = *this - scalar);
    }
    inline vec2<float>& operator-=(const vec2<float>& v)
    {
        return (*this = *this - v);
    }

    template<typename U>
    inline vec2<float>& operator*=(U scalar)
    {
        return (*this = *this * scalar);
    }
    inline vec2<float>& operator*=(const vec2<float>& v)
    {
        return (*this = *this * v);
    }

    template<typename U>
    inline vec2<float>& operator/=(U scalar)
    {
        return (*this = *this / scalar);
    }
    inline vec2<float>& operator/=(const vec2<float>& v)
    {
        return (*this = *this / v);
    }

#ifndef ENABLE_SIMD
    inline vec2<float>& operator++()
    {
        ++this->x;
        ++this->y;
        return *this;
    }

    inline vec2<float>& operator--()
    {
        --this->x;
        --this->y;
        return *this;
    }
#else
    inline vec2<float>& operator++()
    {
        this->vec = SIMD_SSE::increment(this->vec);
        return *this;
    }

    inline vec2<float>& operator--()
    {
        this->vec = SIMD_SSE::decrement(this->vec);
        return *this;
    }
#endif

    inline vec2<float> operator++(int)
    {
        vec2<float> result(*this);
        ++*this;
        return result;
    }
    inline vec2<float> operator--(int)
    {
        vec2<float> result(*this);
        --*this;
        return result;
    }

};

inline vec2<float> operator+(const vec2<float>& v)
{
    return v;
}

inline vec2<float> operator-(const vec2<float>& v)
{
    return vec2<float>(-v.x, -v.y);
}

#ifndef ENABLE_SIMD
inline vec2<float> operator+(const vec2<float>& v, float scalar)
{
    return vec2<float>(
        v.x + static_cast<float>(scalar),
        v.y + static_cast<float>(scalar)
    );
}

inline vec2<float> operator+(float scalar, const vec2<float>& v)
{
    return vec2<float>(
        static_cast<float>(scalar) + v.x,
        static_cast<float>(scalar) + v.y
    );
}

inline vec2<float> operator+(const vec2<float>& v1, const vec2<float>& v2)
{
    return vec2<float>(
        v1.x + v2.x,
        v1.y + v2.y
    );
}

inline vec2<float> operator-(const vec2<float>& v, float scalar)
{
    return vec2<float>(
        v.x - static_cast<float>(scalar),
        v.y - static_cast<float>(scalar)
    );
}

inline vec2<float> operator-(float scalar, const vec2<float>& v)
{
    return vec2<float>(
        static_cast<float>(scalar) - v.x,
        static_cast<float>(scalar) - v.y
    );
}

inline vec2<float> operator-(const vec2<float>& v1, const vec2<float>& v2)
{
    return vec2<float>(
        v1.x - v2.x,
        v1.y - v2.y
    );
}

inline vec2<float> operator*(const vec2<float>& v, float scalar)
{
    return vec2<float>(
        v.x * static_cast<float>(scalar),
        v.y * static_cast<float>(scalar)
    );
}

inline vec2<float> operator*(float scalar, const vec2<float>& v)
{
    return vec2<float>(
        static_cast<float>(scalar) * v.x,
        static_cast<float>(scalar) * v.y
    );
}

inline vec2<float> operator*(const vec2<float>& v1, const vec2<float>& v2)
{
    return vec2<float>(
        v1.x * v2.x,
        v1.y * v2.y
    );
}

inline vec2<float> operator/(const vec2<float>& v, float scalar)
{
    return vec2<float>(
        v.x / static_cast<float>(scalar),
        v.y / static_cast<float>(scalar)
    );
}

inline vec2<float> operator/(float scalar, const vec2<float>& v)
{
    return vec2<float>(
        static_cast<float>(scalar) / v.x,
        static_cast<float>(scalar) / v.y
    );
}

inline vec2<float> operator/(const vec2<float>& v1, const vec2<float>& v2)
{
    return vec2<float>(
        v1.x / v2.x,
        v1.y / v2.y
    );
}

inline float dot(const vec2<float>& v1, const vec2<float>& v2)
{
    return (v1.x * v2.x +
        v1.y * v2.y);
}

#else

inline vec2<float> operator+(const vec2<float>& v, float scalar)
{
    vec2<float> result;
    result.vec = SIMD_SSE::add(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec2<float> operator+(float scalar, const vec2<float>& v)
{
    vec2<float> result;
    result.vec = SIMD_SSE::add(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec2<float> operator+(const vec2<float>& v1, const vec2<float>& v2)
{
    vec2<float> result;
    result.vec = SIMD_SSE::add(v1.vec, v2.vec);
    return result;
}

inline vec2<float> operator-(const vec2<float>& v, float scalar)
{
    vec2<float> result;
    result.vec = SIMD_SSE::sub(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec2<float> operator-(float scalar, const vec2<float>& v)
{
    vec2<float> result;
    result.vec = SIMD_SSE::sub(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec2<float> operator-(const vec2<float>& v1, const vec2<float>& v2)
{
    vec2<float> result;
    result.vec = SIMD_SSE::sub(v1.vec, v2.vec);
    return result;
}

inline vec2<float> operator*(const vec2<float>& v, float scalar)
{
    vec2<float> result;
    result.vec = SIMD_SSE::mul(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec2<float> operator*(float scalar, const vec2<float>& v)
{
    vec2<float> result;
    result.vec = SIMD_SSE::mul(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec2<float> operator*(const vec2<float>& v1, const vec2<float>& v2)
{
    vec2<float> result;
    result.vec = SIMD_SSE::mul(v1.vec, v2.vec);
    return result;
}

inline vec2<float> operator/(const vec2<float>& v, float scalar)
{
    vec2<float> result;
    result.vec = SIMD_SSE::div(v.vec, static_cast<float>(scalar));
    return result;
}

inline vec2<float> operator/(float scalar, const vec2<float>& v)
{
    vec2<float> result;
    result.vec = SIMD_SSE::div(static_cast<float>(scalar), v.vec);
    return result;
}

inline vec2<float> operator/(const vec2<float>& v1, const vec2<float>& v2)
{
    vec2<float> result;
    result.vec = SIMD_SSE::div(v1.vec, v2.vec);
    return result;
}

inline float dot(const vec2<float>& v1, const vec2<float>& v2)
{
    vec2<float> result;
    result.vec = SIMD_SSE::mul(v1.vec, v2.vec);
    return result.x + result.y;
}
#endif

inline bool operator==(const vec2<float>& v1, const vec2<float>& v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y);
}

inline bool operator!=(const vec2<float>& v1, const vec2<float>& v2)
{
    return !(v1 == v2);
}

inline float magnitude(const vec2<float>& v)
{
    return std::sqrt(dot(v, v));
}
