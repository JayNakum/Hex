#include "Surface.h"

#include "Instance.h"

#include "platform/win.h"

namespace graphics
{
    Surface::Surface(const Instance& instance, GLFWwindow* window)
        : mInstance{ instance }
        , mVkSurfaceKHR{ VK_NULL_HANDLE }
    {
        const VkResult vkResult = glfwCreateWindowSurface(mInstance.get(), window, nullptr, &mVkSurfaceKHR);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkSurfaceKHR.");
    }

    Surface::~Surface()
    {
        if (mVkSurfaceKHR != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(mInstance.get(), mVkSurfaceKHR, nullptr);
            mVkSurfaceKHR = VK_NULL_HANDLE;
        }
    }

    Surface::Surface(Surface&& other) noexcept
        : mInstance{ other.mInstance }
        , mVkSurfaceKHR{ other.mVkSurfaceKHR }
    {
        other.mVkSurfaceKHR = VK_NULL_HANDLE;
    }
}
