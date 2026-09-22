#pragma once

#include "vkCommon.h"

struct GLFWwindow;

namespace graphics
{
    class Instance;

    class Surface
    {
    public:
        Surface(const Instance& instance, GLFWwindow* window);
        ~Surface();

        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;

        Surface(Surface&& other) noexcept;
        Surface& operator=(Surface&&) = delete;

        inline const VkSurfaceKHR& get() const { return mVkSurfaceKHR; }

    private:
        const Instance& mInstance;

        VkSurfaceKHR mVkSurfaceKHR;
    };
}
