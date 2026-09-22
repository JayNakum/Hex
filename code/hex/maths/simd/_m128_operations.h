#pragma once

#include <intrin.h>

namespace SIMD_SSE
{
    template <typename T>
    inline __m128 toM128(const T scalar)
    {
        return _mm_set1_ps(static_cast<float>(scalar));
    }

    template <typename A, typename B, typename C, typename D>
    inline __m128 toM128(const A v0 = static_cast<A>(0), const B v1 = static_cast<B>(0), const C v2 = static_cast<C>(0), const D v3 = static_cast<D>(0))
    {
        return _mm_setr_ps(static_cast<float>(v0), static_cast<float>(v1), static_cast<float>(v2), static_cast<float>(v3));
    }
}

namespace SIMD_SSE
{
    inline __m128 add(const __m128& v1, const __m128& v2)
    {
        return _mm_add_ps(v1, v2);
    }
    inline __m128 add(const __m128& v, const float scalar)
    {
        return _mm_add_ps(v, toM128<float>(scalar));
    }
    inline __m128 add(const float scalar, const __m128& v)
    {
        return _mm_add_ps(toM128<float>(scalar), v);
    }
    inline __m128 increment(const __m128& v)
    {
        return _mm_add_ps(v, toM128(1.0f));
    }

    inline __m128 sub(const __m128& v1, const __m128& v2)
    {
        return _mm_sub_ps(v1, v2);
    }
    inline __m128 sub(const __m128& v, const float scalar)
    {
        return _mm_sub_ps(v, toM128<float>(scalar));
    }
    inline __m128 sub(const float scalar, const __m128& v)
    {
        return _mm_sub_ps(toM128<float>(scalar), v);
    }
    inline __m128 decrement(const __m128& v)
    {
        return _mm_sub_ps(v, toM128(1.0f));
    }

    inline __m128 mul(const __m128& v1, const __m128& v2)
    {
        return _mm_mul_ps(v1, v2);
    }
    inline __m128 mul(const __m128& v, const float scalar)
    {
        return _mm_mul_ps(v, toM128<float>(scalar));
    }
    inline __m128 mul(const float scalar, const __m128& v)
    {
        return _mm_mul_ps(toM128<float>(scalar), v);
    }

    inline __m128 div(const __m128& v1, const __m128& v2)
    {
        return _mm_div_ps(v1, v2);
    }
    inline __m128 div(const __m128& v, const float scalar)
    {
        return _mm_div_ps(v, toM128<float>(scalar));
    }
    inline __m128 div(const float scalar, const __m128& v)
    {
        return _mm_div_ps(toM128<float>(scalar), v);
    }
}

namespace SIMD_SSE
{
    template <typename T>
    inline __m128i toM128ui(const T scalar)
    {
        return _mm_set1_epi32(static_cast<int>(static_cast<unsigned int>(scalar)));
    }

    template <typename A, typename B, typename C, typename D>
    inline __m128i toM128ui(const A v0 = static_cast<A>(0), const B v1 = static_cast<B>(0), const C v2 = static_cast<C>(0), const D v3 = static_cast<D>(0))
    {
        return _mm_setr_epi32(
            static_cast<int>(static_cast<unsigned int>(v0)),
            static_cast<int>(static_cast<unsigned int>(v1)),
            static_cast<int>(static_cast<unsigned int>(v2)),
            static_cast<int>(static_cast<unsigned int>(v3))
        );
    }
}

namespace SIMD_SSE
{
    inline __m128i add(const __m128i& v1, const __m128i& v2)
    {
        return _mm_add_epi32(v1, v2);
    }
    inline __m128i add(const __m128i& v, const unsigned int scalar)
    {
        return _mm_add_epi32(v, toM128ui<unsigned int>(scalar));
    }
    inline __m128i add(const unsigned int scalar, const __m128i& v)
    {
        return _mm_add_epi32(toM128ui<unsigned int>(scalar), v);
    }
    inline __m128i increment(const __m128i& v)
    {
        return _mm_add_epi32(v, toM128ui(1.0f));
    }

    inline __m128i sub(const __m128i& v1, const __m128i& v2)
    {
        return _mm_sub_epi32(v1, v2);
    }
    inline __m128i sub(const __m128i& v, const unsigned int scalar)
    {
        return _mm_sub_epi32(v, toM128ui<unsigned int>(scalar));
    }
    inline __m128i sub(const unsigned int scalar, const __m128i& v)
    {
        return _mm_sub_epi32(toM128ui<unsigned int>(scalar), v);
    }
    inline __m128i decrement(const __m128i& v)
    {
        return _mm_sub_epi32(v, toM128ui(1.0f));
    }

    inline __m128i mul(const __m128i& v1, const __m128i& v2)
    {
        return _mm_mullo_epi32(v1, v2);
    }
    inline __m128i mul(const __m128i& v, const unsigned int scalar)
    {
        return _mm_mullo_epi32(v, toM128ui<unsigned int>(scalar));
    }
    inline __m128i mul(const unsigned int scalar, const __m128i& v)
    {
        return _mm_mullo_epi32(toM128ui<unsigned int>(scalar), v);
    }

    inline __m128i div(const __m128i& v1, const __m128i& v2)
    {
        return _mm_cvttps_epi32(_mm_div_ps(_mm_cvtepi32_ps(v1), _mm_cvtepi32_ps(v2)));
    }
    inline __m128i div(const __m128i& v, const unsigned int scalar)
    {
        return _mm_cvttps_epi32(_mm_div_ps(_mm_cvtepi32_ps(v), toM128<unsigned int>(scalar)));
    }
    inline __m128i div(const unsigned int scalar, const __m128i& v)
    {
        return _mm_cvttps_epi32(_mm_div_ps(toM128<unsigned int>(scalar), _mm_cvtepi32_ps(v)));
    }
}
