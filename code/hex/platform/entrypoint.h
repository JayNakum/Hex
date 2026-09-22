#pragma once

#include "core/common.h"

#include "core/Application.h"
#include "Window.h"

#undef CREATE_APPLICATION
#define CREATE_APPLICATION(app_class) \
    hex::Application* createApplication() \
    { \
        return new app_class(); \
    }

extern hex::Application* createApplication();

int main()
{
    try
    {
        platform::Window::setup();

        hex::Application* application = createApplication();

        application->initialize();
        application->run();
        application->deinitialize();

        delete application;

        platform::Window::cleanup();
    }
    catch (std::exception& e)
    {
        HEX_ASSERT(false, e.what());
        return -1;
    }

    return 0;
}
