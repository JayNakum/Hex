#include "Swapchain.h"

#include "PhysicalDevice.h"
#include "Device.h"

namespace graphics
{
    namespace
    {
        VkSemaphore createSemaphore(const Device& device)
        {
            VkSemaphoreCreateInfo vkSemaphoreCreateInfo{};
            vkSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkSemaphore vkSemaphore = VK_NULL_HANDLE;
            const VkResult vkResult = vkCreateSemaphore(device.get(), &vkSemaphoreCreateInfo, nullptr, &vkSemaphore);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkSemaphore. ", utility::translateErrorCode(vkResult).c_str());

            return vkSemaphore;
        }
    }

    Swapchain::Swapchain(const Device& device, const CreateInfo& createInfo)
        : mDevice{ device }
        , mPhysicalDevice{ createInfo.physicalDevice }
        , mVkSurfaceKHR{ createInfo.vkSurfaceKHR }
        , mQueueFamilyIndices{ createInfo.queueFamilyIndices }
        , mVkSwapchainKHR{ VK_NULL_HANDLE }
        , mAcquireSlot{ 0 }
        , mVkExtent2D{}
        , mVkFormat{ VK_FORMAT_UNDEFINED }
    {
        create(createInfo.width, createInfo.height, VK_NULL_HANDLE);
    }

    Swapchain::~Swapchain()
    {
        destroyBackBuffers();

        vkDestroySwapchainKHR(mDevice.get(), mVkSwapchainKHR, nullptr);
        mVkSwapchainKHR = VK_NULL_HANDLE;
    }

    Swapchain::Swapchain(Swapchain&& other) noexcept
        : mDevice{ other.mDevice }
        , mPhysicalDevice{ other.mPhysicalDevice }
        , mVkSurfaceKHR{ other.mVkSurfaceKHR }
        , mQueueFamilyIndices{ std::move(other.mQueueFamilyIndices) }
        , mVkSwapchainKHR{ other.mVkSwapchainKHR }
        , mBackBuffers{ std::move(other.mBackBuffers) }
        , mVkSemaphoresImageAvailable{ std::move(other.mVkSemaphoresImageAvailable) }
        , mAcquireSlot{ other.mAcquireSlot }
        , mVkExtent2D{ other.mVkExtent2D }
        , mVkFormat{ other.mVkFormat }
    {
        other.mVkSwapchainKHR = VK_NULL_HANDLE;
    }

    bool Swapchain::acquireNextBackBuffer(BackBuffer& outBackBuffer)
    {
        const VkSemaphore vkSemaphoreImageAvailable = mVkSemaphoresImageAvailable[mAcquireSlot];
        mAcquireSlot = (mAcquireSlot + 1) % static_cast<uint32_t>(mVkSemaphoresImageAvailable.size());

        uint32_t imageIndex = 0;
        const VkResult vkResult = vkAcquireNextImageKHR(mDevice.get(), mVkSwapchainKHR, UINT64_MAX, vkSemaphoreImageAvailable, VK_NULL_HANDLE, &imageIndex);

        if (vkResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return false;
        }

        HEX_ASSERT(vkResult == VK_SUCCESS || vkResult == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image. ", utility::translateErrorCode(vkResult).c_str());

        outBackBuffer = mBackBuffers[imageIndex];
        outBackBuffer.imageAvailable = vkSemaphoreImageAvailable;
        return true;
    }

    bool Swapchain::present(VkQueue vkQueue, const BackBuffer& backBuffer) const
    {
        VkPresentInfoKHR vkPresentInfoKHR{};
        vkPresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        vkPresentInfoKHR.waitSemaphoreCount = 1;
        vkPresentInfoKHR.pWaitSemaphores = &backBuffer.renderFinished;
        vkPresentInfoKHR.swapchainCount = 1;
        vkPresentInfoKHR.pSwapchains = &mVkSwapchainKHR;
        vkPresentInfoKHR.pImageIndices = &backBuffer.imageIndex;

        const VkResult vkResult = vkQueuePresentKHR(vkQueue, &vkPresentInfoKHR);

        if (vkResult == VK_ERROR_OUT_OF_DATE_KHR || vkResult == VK_SUBOPTIMAL_KHR)
        {
            return false;
        }

        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to present swapchain image. ", utility::translateErrorCode(vkResult).c_str());
        return true;
    }

    void Swapchain::resize(const uint32_t width, const uint32_t height)
    {
        const VkSwapchainKHR vkSwapchainKHROld = mVkSwapchainKHR;
        create(width, height, vkSwapchainKHROld);

        vkDestroySwapchainKHR(mDevice.get(), vkSwapchainKHROld, nullptr);
    }

    void Swapchain::create(const uint32_t width, const uint32_t height, const VkSwapchainKHR vkSwapchainKHROld)
    {
        const PhysicalDevice::SurfaceSupportDetails surfaceSupportDetails = mPhysicalDevice.querySurfaceSupportDetails(mVkSurfaceKHR);

        const VkSurfaceFormatKHR vkSurfaceFormatKHRSwapchain = chooseSurfaceFormat(surfaceSupportDetails.formats);
        const VkPresentModeKHR vkPresentModeKHR = choosePresentMode(surfaceSupportDetails.presentModes);
        const VkExtent2D vkExtent2DSwapchain = chooseExtent(surfaceSupportDetails.capabilities, width, height);

        const VkSurfaceCapabilitiesKHR& vkSurfaceCapabilitiesKHR = surfaceSupportDetails.capabilities;
        const uint32_t maxImages = (vkSurfaceCapabilitiesKHR.maxImageCount > 0) ? vkSurfaceCapabilitiesKHR.maxImageCount : UINT32_MAX;
        const uint32_t imageCount = std::clamp(BACK_BUFFER_COUNT, vkSurfaceCapabilitiesKHR.minImageCount, maxImages);

        VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKHR{};
        vkSwapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        vkSwapchainCreateInfoKHR.surface = mVkSurfaceKHR;
        vkSwapchainCreateInfoKHR.minImageCount = imageCount;
        vkSwapchainCreateInfoKHR.imageFormat = vkSurfaceFormatKHRSwapchain.format;
        vkSwapchainCreateInfoKHR.imageColorSpace = vkSurfaceFormatKHRSwapchain.colorSpace;
        vkSwapchainCreateInfoKHR.imageExtent = vkExtent2DSwapchain;
        vkSwapchainCreateInfoKHR.imageArrayLayers = 1;
        vkSwapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const std::vector<uint32_t> queueFamilyIndices(mQueueFamilyIndices.begin(), mQueueFamilyIndices.end());
        HEX_ASSERT(!queueFamilyIndices.empty(), "Need at least one command queue with swapchain support.");

        if (queueFamilyIndices.size() > 1)
        {
            vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            vkSwapchainCreateInfoKHR.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
            vkSwapchainCreateInfoKHR.pQueueFamilyIndices = queueFamilyIndices.data();
        }
        else
        {
            vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            vkSwapchainCreateInfoKHR.queueFamilyIndexCount = 0;
            vkSwapchainCreateInfoKHR.pQueueFamilyIndices = nullptr;
        }

        vkSwapchainCreateInfoKHR.preTransform = vkSurfaceCapabilitiesKHR.currentTransform;
        vkSwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        vkSwapchainCreateInfoKHR.presentMode = vkPresentModeKHR;
        vkSwapchainCreateInfoKHR.clipped = VK_TRUE;
        vkSwapchainCreateInfoKHR.oldSwapchain = vkSwapchainKHROld;

        const VkResult vkResult = vkCreateSwapchainKHR(mDevice.get(), &vkSwapchainCreateInfoKHR, nullptr, &mVkSwapchainKHR);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkSwapchainKHR. ", utility::translateErrorCode(vkResult).c_str());

        mVkExtent2D = vkExtent2DSwapchain;
        mVkFormat = vkSurfaceFormatKHRSwapchain.format;

        destroyBackBuffers();
        createBackBuffers();
    }

    void Swapchain::createBackBuffers()
    {
        uint32_t swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(mDevice.get(), mVkSwapchainKHR, &swapchainImageCount, nullptr);

        std::vector<VkImage> vkImages(swapchainImageCount);
        vkGetSwapchainImagesKHR(mDevice.get(), mVkSwapchainKHR, &swapchainImageCount, vkImages.data());

        mBackBuffers.resize(swapchainImageCount);
        for (uint32_t i = 0; i < swapchainImageCount; ++i)
        {
            VkImageViewCreateInfo vkImageViewCreateInfo{};
            vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            vkImageViewCreateInfo.image = vkImages[i];
            vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            vkImageViewCreateInfo.format = mVkFormat;
            vkImageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
            vkImageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

            VkImageView vkImageView = VK_NULL_HANDLE;
            const VkResult vkResult = vkCreateImageView(mDevice.get(), &vkImageViewCreateInfo, nullptr, &vkImageView);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create back buffer VkImageView. ", utility::translateErrorCode(vkResult).c_str());

            mBackBuffers[i].image = vkImages[i];
            mBackBuffers[i].view = vkImageView;
            mBackBuffers[i].extent = mVkExtent2D;
            mBackBuffers[i].renderFinished = createSemaphore(mDevice);
            mBackBuffers[i].imageIndex = i;
        }

        mVkSemaphoresImageAvailable.resize(swapchainImageCount);
        for (uint32_t i = 0; i < swapchainImageCount; ++i)
        {
            mVkSemaphoresImageAvailable[i] = createSemaphore(mDevice);
        }
        mAcquireSlot = 0;
    }

    void Swapchain::destroyBackBuffers()
    {
        for (VkSemaphore vkSemaphore : mVkSemaphoresImageAvailable)
        {
            vkDestroySemaphore(mDevice.get(), vkSemaphore, nullptr);
        }
        mVkSemaphoresImageAvailable.clear();

        for (BackBuffer& backBuffer : mBackBuffers)
        {
            vkDestroySemaphore(mDevice.get(), backBuffer.renderFinished, nullptr);
            vkDestroyImageView(mDevice.get(), backBuffer.view, nullptr);
        }
        mBackBuffers.clear();
    }

    VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& vkSurfaceFormatsAvailable) const
    {
        for (const VkSurfaceFormatKHR& vkSurfaceFormatKHR : vkSurfaceFormatsAvailable)
        {
            if (vkSurfaceFormatKHR.format == VK_FORMAT_B8G8R8A8_SRGB && vkSurfaceFormatKHR.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return vkSurfaceFormatKHR;
            }
        }

        return vkSurfaceFormatsAvailable[0];
    }

    VkPresentModeKHR Swapchain::choosePresentMode(const std::vector<VkPresentModeKHR>& vkPresentModesAvailable) const
    {
        for (const VkPresentModeKHR& vkPresentModeKHR : vkPresentModesAvailable)
        {
            if (vkPresentModeKHR == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return vkPresentModeKHR;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& vkSurfaceCapabilitiesKHR, const uint32_t width, const uint32_t height) const
    {
        if (vkSurfaceCapabilitiesKHR.currentExtent.width != UINT32_MAX)
        {
            return vkSurfaceCapabilitiesKHR.currentExtent;
        }
        else
        {
            VkExtent2D vkExtent2DActual = { width, height };

            vkExtent2DActual.width = std::clamp(vkExtent2DActual.width, vkSurfaceCapabilitiesKHR.minImageExtent.width, vkSurfaceCapabilitiesKHR.maxImageExtent.width);
            vkExtent2DActual.height = std::clamp(vkExtent2DActual.height, vkSurfaceCapabilitiesKHR.minImageExtent.height, vkSurfaceCapabilitiesKHR.maxImageExtent.height);

            return vkExtent2DActual;
        }
    }
}
