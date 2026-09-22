#pragma once

#include "../vectors/vec3.h"
#include "../vectors/vec4.h"
#include "../matrices/mat3x3.h"
#include "../matrices/mat4x4.h"

template <typename T>
class qua
{
public:
    struct
    {
        T w, x, y, z;
    };

public:
    qua()
        : w(0), x(0), y(0), z(0){}
    qua(const qua<T>& q)
        : w(q.w), x(q.x), y(q.y), z(q.z) {}
    qua(T s, const vec3<T>& v)
        : w(s), x(v.x), y(v.y), z(v.z) {}
    qua(T _w, T _x, T _y, T _z)
        : w(_w), x(_x), y(_y), z(_z) {}
    static qua<T> wxyz(T w, T x, T y, T z)
    {
        return qua<T>(w, x, y, z);
    }
    template <typename U>
    qua(const qua<U>& q)
        : w(static_cast<T>(q.w)), x(static_cast<T>(q.x)), y(static_cast<T>(q.y)), z(static_cast<T>(q.z)) {}

    qua(const vec3<T>& eulerAngles)
    {
        vec3<T> c(std::cos(eulerAngles.x) * T(0.5), std::cos(eulerAngles.y) * T(0.5), std::cos(eulerAngles.z) * T(0.5));
        vec3<T> s(std::sin(eulerAngles.x) * T(0.5), std::sin(eulerAngles.y) * T(0.5), std::sin(eulerAngles.z) * T(0.5));

        this->w = c.x * c.y * c.z + s.x * s.y * s.z;
        this->x = s.x * c.y * c.z - c.x * s.y * s.z;
        this->y = c.x * s.y * c.z + s.x * c.y * s.z;
        this->z = c.x * c.y * s.z - s.x * s.y * c.z;
    }
    template <typename A, typename B, typename C>
    qua(A roll, B pitch, C yaw)
    {
        vec3<T> c(std::cos(static_cast<T>(roll)) * T(0.5), std::cos(static_cast<T>(pitch)) * T(0.5), std::cos(static_cast<T>(yaw)) * T(0.5));
        vec3<T> s(std::sin(static_cast<T>(roll)) * T(0.5), std::sin(static_cast<T>(pitch)) * T(0.5), std::sin(static_cast<T>(yaw)) * T(0.5));

        this->w = c.x * c.y * c.z + s.x * s.y * s.z;
        this->x = s.x * c.y * c.z - c.x * s.y * s.z;
        this->y = c.x * s.y * c.z + s.x * c.y * s.z;
        this->z = c.x * c.y * s.z - s.x * s.y * c.z;
    }
    qua(const mat3x3<T>& m)
    {
        *this = qua<T>::fromMat3(m);
    }
    qua(const mat4x4<T>& m)
    {
        *this = qua<T>::fromMat4(m);
    }

    static const size_t length = 4;

public:
    inline const T& operator[](size_t i) const
    {
        if (i >= qua::length)
            throw std::out_of_range("qua");
        return (&w)[i];
    }
    inline T& operator[](size_t i)
    {
        if (i >= qua::length)
            throw std::out_of_range("qua");
        return (&w)[i];
    }

    inline qua<T>& operator=(const qua<T>& q)
    {
        this->w = q.w;
        this->x = q.x;
        this->y = q.y;
        this->z = q.z;
        return *this;
    }
    template <typename U>
    inline qua<T>& operator=(const qua<U>& q)
    {
        this->w = static_cast<T>(q.w);
        this->x = static_cast<T>(q.x);
        this->y = static_cast<T>(q.y);
        this->z = static_cast<T>(q.z);
        return *this;
    }

    template <typename U>
    inline qua<T>& operator+=(const qua<T>& q)
    {
        return (*this = qua<T>::wxyz(
            w + static_cast<T>(q.w),
            x + static_cast<T>(q.x),
            y + static_cast<T>(q.y),
            z + static_cast<T>(q.z))
        );
    }

    template <typename U>
    inline qua<T>& operator-=(const qua<T>& q)
    {
        return (*this = qua<T>::wxyz(
            w - static_cast<T>(q.w),
            x - static_cast<T>(q.x),
            y - static_cast<T>(q.y),
            z - static_cast<T>(q.z))
        );
    }

    template <typename U>
    inline qua<T>& operator*=(U s)
    {
        return (*this = qua<T>::wxyz(
            w * static_cast<T>(s),
            x * static_cast<T>(s),
            y * static_cast<T>(s),
            z * static_cast<T>(s))
        );
    }
    template <typename U>
    inline qua<T>& operator*=(const qua<U>& r)
    {
        const qua<T> p(*this);
        const qua<T> q(r);

        this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
        this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
        this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
        this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
        return *this;
    }

    template <typename U>
    inline qua<T>& operator/=(U s)
    {
        *this = qua<T>::wxyz(
            w / static_cast<T>(s),
            x / static_cast<T>(s),
            y / static_cast<T>(s),
            z / static_cast<T>(s)
        );
        return *this;
    }

public:
    static qua<T> fromMat3(const mat3x3<T>& m)
    {
        T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
        T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
        T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
        T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

        int biggestIndex = 0;
        T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
        if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }

        T biggestVal = std::sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1)) * static_cast<T>(0.5);
        T mult = static_cast<T>(0.25) / biggestVal;

        switch (biggestIndex)
        {
        case 0:
            return qua<T>::wxyz(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);
        case 1:
            return qua<T>::wxyz((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);
        case 2:
            return qua<T>::wxyz((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);
        case 3:
            return qua<T>::wxyz((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);
        default:
            return qua<T>::wxyz(1, 0, 0, 0);
        }
    }

    static qua<T> fromMat4(const mat4x4<T>& m)
    {
        return fromMat3(mat3x3<T>(
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2]
        ));
    }

    static mat3x3<T> toMat3(const qua<T>& q)
    {
        mat3x3<T> result(T(1));
        T qxx(q.x * q.x);
        T qyy(q.y * q.y);
        T qzz(q.z * q.z);
        T qxz(q.x * q.z);
        T qxy(q.x * q.y);
        T qyz(q.y * q.z);
        T qwx(q.w * q.x);
        T qwy(q.w * q.y);
        T qwz(q.w * q.z);

        result[0][0] = T(1) - T(2) * (qyy + qzz);
        result[0][1] = T(2) * (qxy + qwz);
        result[0][2] = T(2) * (qxz - qwy);

        result[1][0] = T(2) * (qxy - qwz);
        result[1][1] = T(1) - T(2) * (qxx + qzz);
        result[1][2] = T(2) * (qyz + qwx);

        result[2][0] = T(2) * (qxz + qwy);
        result[2][1] = T(2) * (qyz - qwx);
        result[2][2] = T(1) - T(2) * (qxx + qyy);
        return result;
    }

    static mat4x4<T> toMat4(const qua<T>& q)
    {
        mat3x3<T> m = toMat3(q);
        return mat4x4<T>(
            m[0][0], m[0][1], m[0][2], T(0),
            m[1][0], m[1][1], m[1][2], T(0),
            m[2][0], m[2][1], m[2][2], T(0),
            T(0),	 T(0),	  T(0),	   T(1)
        );
    }

    static vec3<T> toEulerAngles(const qua<T>& q)
    {
        vec3<T> angles;

        T sinr_cosp = static_cast<T>(2) * (q.w * q.x + q.y * q.z);
        T cosr_cosp = static_cast<T>(1) - static_cast<T>(2) * (q.x * q.x + q.y * q.y);
        angles.x = std::atan2(sinr_cosp, cosr_cosp);

        T sinp = std::sqrt(static_cast<T>(1) + static_cast<T>(2) * (q.w * q.y - q.x * q.z));
        T cosp = std::sqrt(static_cast<T>(1) - static_cast<T>(2) * (q.w * q.y - q.x * q.z));
        angles.y = static_cast<T>(2) * std::atan2(sinp, cosp) - static_cast<T>(M_PI) / static_cast<T>(2);

        T siny_cosp = static_cast<T>(2) * (q.w * q.z + q.x * q.y);
        T cosy_cosp = static_cast<T>(1) - static_cast<T>(2) * (q.y * q.y + q.z * q.z);
        angles.z = std::atan2(siny_cosp, cosy_cosp);

        return angles;
    }
};

template <typename T>
inline qua<T> operator+(const qua<T>& q)
{
    return q;
}

template <typename T>
inline qua<T> operator-(const qua<T>& q)
{
    return qua<T>::wxyz(-q.w, -q.x, -q.y, -q.z);
}

template <typename T>
inline qua<T> operator+(const qua<T>& q, const qua<T>& p)
{
    return qua<T>(q) += p;
}

template <typename T>
inline qua<T> operator-(const qua<T>& q, const qua<T>& p)
{
    return qua<T>(q) -= p;
}

template <typename T>
inline qua<T> operator*(const qua<T>& q, const qua<T>& p)
{
    return qua<T>(q) *= p;
}
template <typename T>
inline qua<T> operator*(const qua<T>& q, T s)
{
    return qua<T>(q) *= s;
}
template <typename T>
inline vec3<T> operator*(const qua<T>& q, const vec3<T>& v)
{
    const vec3<T> qVec(q.x, q.y, q.z);
    const vec3<T> uv(cross(qVec, v));
    const vec3<T> uuv(cross(qVec, uv));

    return v + ((uv * q.w) + uuv) * static_cast<T>(2);
}
template <typename T>
inline vec3<T> operator*(const vec3<T>& v, const qua<T>& q)
{
    return inverse(q) * v;
}

template <typename T>
inline qua<T> operator/(const qua<T>& q, T s)
{
    return qua<T>(q) /= s;
}

template <typename T>
inline bool operator==(const qua<T>& q1, const qua<T>& q2)
{
    return (q1.w == q2.w) && (q1.x == q2.x) && (q1.y == q2.y) && (q1.z == q2.z);
}

template <typename T>
inline bool operator!=(const qua<T>& q1, const qua<T>& q2)
{
    return !(q1 == q2);
}

template <typename T>
inline T dot(const qua<T>& q1, const qua<T>& q2)
{
    vec4<T> tmp(q1.w * q2.w, q1.x * q2.x, q1.y * q2.y, q1.z * q2.z);
    return (tmp.x + tmp.y) + (tmp.z + tmp.w);
}

template <typename T>
inline T magnitude(const qua<T>& q)
{
    return std::sqrt(
        q.w * q.w +
        q.x * q.x +
        q.y * q.y +
        q.z * q.z
    );
}

template <typename T>
inline qua<T> conjugate(const qua<T>& q)
{
    return qua<T>::wxyz(q.w, -q.x, -q.y, -q.z);
}

template <typename T>
inline qua<T> inverse(const qua<T>& q)
{
    return conjugate<T>(q) / magnitude<T>(q);
}
