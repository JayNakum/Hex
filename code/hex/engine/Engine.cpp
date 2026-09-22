#include "Engine.h"

#include "graphics/Renderer.h"

namespace engine
{
    uint64_t Engine::msFrameIndex = 0;

    Engine::Engine(platform::Window& mainWindow)
        : mWindow{ mainWindow }
        , mInputs{ mWindow.get() }
        , mRenderer{ nullptr }
    {
        mRenderer = new graphics::Renderer(mWindow);
    }

    Engine::~Engine()
    {
        delete mRenderer;
    }

    void Engine::run()
    {
        graphics::FrameConstants frame;

        while (!mInputs.isPressed(hex::Inputs::Digital::Key_escape) && mWindow.pollEvents())
        {
            frame.deltaTime = mFrameTimer.restart();
            frame.totalTime = mFrameTimer.total();
            frame.width = mWindow.getDescription().width;
            frame.height = mWindow.getDescription().height;

            mInputs.update(frame.deltaTime);

            if (mWindow.didResize() && (mWindow.getDescription().width != 0 && mWindow.getDescription().height != 0))
            {
                mRenderer->resize(mWindow.getDescription().width, mWindow.getDescription().height);
            }
            else
            {
                static double accumulator = 0.0;
                constexpr double fixedTimeStep = 1.0 / 60.0;
                accumulator += static_cast<double>(frame.deltaTime);

                while (accumulator >= fixedTimeStep)
                {
                    accumulator -= fixedTimeStep;
                    mRenderer->update(fixedTimeStep);
                }

                mRenderer->render(frame);

                ++msFrameIndex;

                calculateFPS(frame.deltaTime);
            }
        }
    }

    void Engine::calculateFPS(const float deltaTime)
    {
        static float fpsTimer = 0.0f;
        static unsigned int frameCount = 0;

        ++frameCount;
        fpsTimer += deltaTime;

        if (fpsTimer >= 1.0f)
        {
            const unsigned int fps = frameCount;
            const float msPerFrame = 1000.0f / static_cast<float>(std::max<unsigned int>(fps, 1u));

            const std::string fpsString = " | FPS " + std::to_string(fps) +
                " (" + std::to_string(msPerFrame) + "ms)";

            mWindow.appendWindowText(fpsString);

            fpsTimer = 0.0f;
            frameCount = 0;
        }
    }
}
