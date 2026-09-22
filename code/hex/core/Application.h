#pragma once

#include "common.h"

namespace hex
{
    class Application
    {
    public:
        virtual ~Application() = default;

        virtual void initialize() = 0;
        virtual void run() = 0;
        virtual void deinitialize() = 0;
    };
}
