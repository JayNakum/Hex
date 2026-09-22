#pragma once

#include "core/common.h"

#include "platform/Window.h"
#include "platform/Inputs.h"
#include "Timer.h"

namespace graphics
{
    class Renderer;
}

namespace engine
{
    class Engine
    {
    public:
        Engine(platform::Window& mainWindow);
        ~Engine();

        inline static uint64_t getFrameIndex() { return msFrameIndex; }

        void run();

    private:
        void calculateFPS(const float deltaTime);

    private:
        platform::Window& mWindow;
        platform::Inputs mInputs;
        graphics::Renderer* mRenderer;

        static uint64_t msFrameIndex;
        Timer mFrameTimer;
    };
}
