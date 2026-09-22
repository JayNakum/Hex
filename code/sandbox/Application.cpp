#include "core/common.h"

#include "platform/entrypoint.h"
#include "platform/Window.h"
#include "engine/Engine.h"

namespace sandbox
{
    class Application : public hex::Application
    {
    public:
        Application()
            : mWindow(platform::Window::Description{})
            , mEngine(mWindow)
        {
        }

        void initialize() override
        {
        }

        void run() override
        {
            mEngine.run();
        }

        void deinitialize() override
        {
        }

        ~Application() override
        {
        }

    private:
        platform::Window mWindow;
        engine::Engine mEngine;
    };
}

CREATE_APPLICATION(sandbox::Application);
