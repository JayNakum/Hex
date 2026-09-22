#pragma once

#include "vkCommon.h"

#include "PhysicalDevice.h"

namespace graphics
{
    class Device;

    class Swapchain
    {
    public:
        static constexpr uint32_t BACK_BUFFER_COUNT = 3;

        struct CreateInfo
        {
            const PhysicalDevice& physicalDevice;
            VkSurfaceKHR vkSurfaceKHR;

            std::unordered_set<uint32_t> queueFamilyIndices;

            uint32_t width;
            uint32_t height;
        };

        struct BackBuffer
        {
            VkImage image = VK_NULL_HANDLE;
            VkImageView view = VK_NULL_HANDLE;
            VkExtent2D extent = {};
            VkSemaphore renderFinished = VK_NULL_HANDLE;
            VkSemaphore imageAvailable = VK_NULL_HANDLE;
            uint32_t imageIndex = 0;
        };

        Swapchain(const Device& device, const CreateInfo& createInfo);
        ~Swapchain();

        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        Swapchain(Swapchain&& other) noexcept;
        Swapchain& operator=(Swapchain&&) = delete;

        inline const VkSwapchainKHR& get() const { return mVkSwapchainKHR; }

        inline size_t getImageCount() const { return mBackBuffers.size(); }

        inline VkExtent2D getExtent() const { return mVkExtent2D; }
        inline VkFormat getFormat() const { return mVkFormat; }

        bool acquireNextBackBuffer(BackBuffer& outBackBuffer);
        bool present(VkQueue vkQueue, const BackBuffer& backBuffer) const;

        void resize(const uint32_t width, const uint32_t height);

    private:
        void create(const uint32_t width, const uint32_t height, const VkSwapchainKHR vkSwapchainKHROld);
        void createBackBuffers();
        void destroyBackBuffers();

        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& vkSurfaceFormatsAvailable) const;
        VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& vkPresentModesAvailable) const;
        VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& vkSurfaceCapabilitiesKHR, const uint32_t width, const uint32_t height) const;

    private:
        const Device& mDevice;
        const PhysicalDevice& mPhysicalDevice;
        VkSurfaceKHR mVkSurfaceKHR;
        std::unordered_set<uint32_t> mQueueFamilyIndices;

        VkSwapchainKHR mVkSwapchainKHR;
        std::vector<BackBuffer> mBackBuffers;
        std::vector<VkSemaphore> mVkSemaphoresImageAvailable;
        uint32_t mAcquireSlot;
        VkExtent2D mVkExtent2D;
        VkFormat mVkFormat;
    };
}
