#pragma once

#include "utility.h"

inline void hexLog(const char* category,
    const char* file,
    const char* func,
    int line,
    const std::string message)
{
    utility::print(
        utility::stringFormat(
            "[", category, "] ",
            utility::timeNow(), " | ",
            file, " | ",
            func, ":", line, " | ",
            message,
            "\n"
        )
    );

    std::fflush(stdout);
}

#define HEX_LOG(category, ...) hexLog(#category, __FILE__, __func__, __LINE__, utility::stringFormat(__VA_ARGS__))

#if defined(HEX_DEBUG)

#if defined(_MSC_VER)
    #define DEBUG_BREAK() __debugbreak() // Windows (MSVC)
#elif defined(__GNUC__) || defined(__clang__)
    #if defined(__x86_64__) || defined(__i386__)
        #define DEBUG_BREAK() __asm__ volatile("int $3") // Intel/AMD x86/x64
    #elif defined(__arm__) || defined(__aarch64__)
        #define DEBUG_BREAK() __builtin_trap() // ARM/Apple Silicon
    #else
        #define DEBUG_BREAK() __builtin_trap() // Fallback
    #endif
#else // defined(__GNUC__) || defined(__clang__)
    #define DEBUG_BREAK() std::abort() // Ultimate fallback (Crash)
#endif // defined(_MSC_VER)

#define HEX_DEBUG_ONLY(...) __VA_ARGS__

#else // defined(HEX_DEBUG)

#define DEBUG_BREAK()

#define HEX_DEBUG_ONLY(...)

#endif // defined(HEX_DEBUG)

#define HEX_SOFT_ASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HEX_LOG(SOFT_ASSERT, "Assertion failed: ", #expression, __VA_ARGS__); \
            DEBUG_BREAK(); \
        } \
    } while (0)

#define HEX_ASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HEX_LOG(ASSERT, "Assertion failed: ", #expression, __VA_ARGS__); \
            DEBUG_BREAK(); \
            std::abort(); \
        } \
    } while (0)
