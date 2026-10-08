#pragma once

#include <optional>

#include "vulkan/vkCommon.h"

#include "platform/Window.h"

#include "vulkan/Instance.h"
#include "vulkan/Surface.h"
#include "vulkan/PhysicalDevice.h"
#include "vulkan/Device.h"

#include "vulkan/Swapchain.h"
#include "vulkan/CommandContext.h"
#include "vulkan/CommandContextPool.h"

#include "ResourceManager.h"
#include "PassCore.h"

#include "RayTracingPass.h"
#include "PresentPass.h"
#include "ImGUI.h"

namespace graphics
{
    class Renderer
    {
    public:
        Renderer(platform::Window& window);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void update(const double timeStep);
        void render(FrameConstants frameConstants);

        void resize(const uint32_t width, const uint32_t height);

    private:
        static Device createDevice(const Instance& instance, const PhysicalDevice& physicalDevice, const QueueFamilies& queueFamilies);

        void waitForFence(VkFence fence) const;
        bool acquireBackBuffer(Swapchain::BackBuffer& backBuffer);

        void submitCopy(FrameConstants frame, const std::vector<VkCommandBufferSubmitInfo>& commandBufferSubmitInfos);
        void submitCompute(FrameConstants frame, const std::vector<VkCommandBufferSubmitInfo>& commandBufferSubmitInfos);
        void submitGraphics(FrameConstants frame, const Swapchain::BackBuffer& backBuffer, const std::vector<VkCommandBufferSubmitInfo>& commandBufferSubmitInfos);

    private:
        static constexpr uint32_t QUEUE_INDEX = 0;

    private:
        platform::Window& mWindow;

        Instance mInstance;
        Surface mSurface;
        PhysicalDevice mPhysicalDevice;

        QueueFamilies mQueueFamilies;
        Device mDevice;

        ResourceManager mResourceManager;

        VkQueue mVkQueueGraphics;
        VkQueue mVkQueueCompute;
        VkQueue mVkQueueCopy;

        std::optional<Swapchain> mSwapchain;

        CommandContextPool mGraphicsContextPool;
        CommandContextPool mComputeContextPool;
        CommandContextPool mCopyContextPool;

        RayTracingPass mRayTracingPass;
        PresentPass mPresentPass;
        ImGuiPass mImGuiPass;

        std::array<VkSemaphore, FRAMES_IN_FLIGHT> mVkSemaphoresComputeFinished;
        std::array<VkSemaphore, FRAMES_IN_FLIGHT> mVkSemaphoresGraphicsReadDone;
        std::array<VkSemaphore, FRAMES_IN_FLIGHT> mVkSemaphoresCopyFinished;

        std::array<bool, FRAMES_IN_FLIGHT> mHasGraphicsReadPending;
        std::array<bool, FRAMES_IN_FLIGHT> mHasCopyPending;

        std::array<VkFence, FRAMES_IN_FLIGHT> mVkFencesGraphics;
        std::array<VkFence, FRAMES_IN_FLIGHT> mVkFencesCompute;
        std::array<VkFence, FRAMES_IN_FLIGHT> mVkFencesCopy;

        std::vector<VkFence> mVkFencesImagesInFlight;

        uint32_t mCurrentFrame;
    };
}
