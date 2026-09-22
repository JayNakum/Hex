#pragma once

#include "core/common.h"

namespace engine
{
    class Timer
    {
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = Clock::time_point;

    public:
        Timer();
        ~Timer();

        float restart();
        float total() const;
        float elapsed() const;
        void reset();

    private:
        TimePoint mStart;
        TimePoint mLast;
    };
}
