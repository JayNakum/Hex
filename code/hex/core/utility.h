#pragma once

#include "pch.h"

namespace utility
{
    template<typename... Args>
    static inline std::string stringFormat(Args&&... args)
    {
        std::ostringstream oss;
        (oss << ... << args);
        return oss.str();
    }

    static inline std::string timeNow()
    {
        using namespace std::chrono;

        const system_clock::time_point now = system_clock::now();
        const std::time_t now_time_t = system_clock::to_time_t(now);

        std::tm tm{};
        localtime_s(&tm, &now_time_t);

        char buffer[16];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm);
        return buffer;
    }

    void print(const std::string& message);

    std::string translateErrorCode(const long errorCode);
}

namespace maths
{
    template <typename T> inline T divideByMultiple(T value, size_t alignment)
    {
        return static_cast<T>((value + alignment - 1) / alignment);
    }
}

namespace memory
{
    template <typename T> inline T alignUpWithMask(T value, size_t mask)
    {
        return static_cast<T>((static_cast<size_t>(value) + mask) & ~mask);
    }

    template <typename T> inline T alignDownWithMask(T value, size_t mask)
    {
        return static_cast<T>(static_cast<size_t>(value) & ~mask);
    }

    template <typename T> inline T alignUp(T value, size_t alignment)
    {
        return alignUpWithMask(value, alignment - 1);
    }

    template <typename T> inline T alignDown(T value, size_t alignment)
    {
        return alignDownWithMask(value, alignment - 1);
    }

    template <typename T> inline bool isAligned(T value, size_t alignment)
    {
        return 0 == (static_cast<size_t>(value) & (alignment - 1));
    }

    constexpr size_t KB(const size_t kilobytes)
    {
        return kilobytes << 10;
    }

    constexpr size_t MB(const size_t megabytes)
    {
        return megabytes << 20;
    }

    constexpr size_t GB(const size_t gigabytes)
    {
        return gigabytes << 30;
    }

    constexpr size_t TB(const size_t terabytes)
    {
        return terabytes << 40;
    }

}

template <typename Enum>
constexpr std::underlying_type_t<Enum> enumCast(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}
