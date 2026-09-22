#pragma once

#include "core/common.h"

struct GLFWwindow;

namespace platform
{
    class Window
    {
    public:
        struct Description
        {
            const std::string title = "Hex";
            unsigned int width = 1920;
            unsigned int height = 1080;
        };

        static void setup();
        static void cleanup();

        Window(const Description& description);
        ~Window();

        inline const Description& getDescription() const { return mDescription; }
        inline GLFWwindow* get() const { return mWindow; }

        inline bool didResize()
        {
            bool result = mDidResize;
            mDidResize = false;
            return result;
        }

        bool pollEvents();

        void appendWindowText(const std::string& text);

    private:
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    private:
        Description mDescription;
        GLFWwindow* mWindow;
        bool mDidResize;
    };
}
