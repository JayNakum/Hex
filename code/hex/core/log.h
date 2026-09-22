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

#define HEX_ASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HEX_LOG(ASSERT, "Assertion failed: ", #expression, __VA_ARGS__); \
            std::abort(); \
        } \
    } while (0)

#define HEX_SOFT_ASSERT(expression, ...) \
    do \
    { \
        if (!(expression)) \
        { \
            HEX_LOG(SOFT_ASSERT, "Assertion failed: ", #expression, __VA_ARGS__); \
        } \
    } while (0)

#if defined(HEX_DEBUG)

#define HEX_DEBUG_ONLY(...) __VA_ARGS__

#else

#define HEX_DEBUG_ONLY(...)

#endif
