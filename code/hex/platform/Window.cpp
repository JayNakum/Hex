#include "Window.h"

#include <GLFW/glfw3.h>

namespace platform
{
    void Window::setup()
    {
        glfwInit();
    }

    void Window::cleanup()
    {
        glfwTerminate();
    }

    Window::Window(const Description& description)
        : mDescription{ description }
        , mWindow{ nullptr }
        , mDidResize{ false }
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        mWindow = glfwCreateWindow(
            static_cast<int>(description.width),
            static_cast<int>(description.height),
            description.title.c_str(),
            nullptr, nullptr
        );

        HEX_ASSERT(mWindow, "Failed to create glfw window.");

        glfwSetWindowUserPointer(mWindow, this);
        glfwSetFramebufferSizeCallback(mWindow, &Window::framebufferSizeCallback);
    }

    Window::~Window()
    {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }

    bool Window::pollEvents()
    {
        glfwPollEvents();
        return !glfwWindowShouldClose(mWindow);
    }

    void Window::appendWindowText(const std::string& text)
    {
        std::string windowText = mDescription.title;
        windowText += text;

        glfwSetWindowTitle(mWindow, windowText.c_str());
    }

    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (self)
        {
            self->mDescription.width = static_cast<unsigned int>(width);
            self->mDescription.height = static_cast<unsigned int>(height);
            self->mDidResize = true;
        }
    }
}
