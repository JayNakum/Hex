#include "Renderer.h"

#include <algorithm>

#include "vulkan/ShaderInput.h"

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

        VkQueue getDeviceQueue(const Device& device, const uint32_t familyIndex, const uint32_t queueIndex)
        {
            VkQueue vkQueue = VK_NULL_HANDLE;
            vkGetDeviceQueue(device.get(), familyIndex, queueIndex, &vkQueue);
            return vkQueue;
        }

        uint32_t computeGraphicsFamilyIndex(const PhysicalDevice& physicalDevice, VkSurfaceKHR vkSurfaceKHR)
        {
            const uint32_t familyIndex = physicalDevice.getGraphicsQueueFamilyIndex();
            HEX_ASSERT(physicalDevice.checkSurfaceSupport(familyIndex, vkSurfaceKHR), "Graphics queue family does not support presentation to the given surface.");

            return familyIndex;
        }

        uint32_t computeQueueCount(const PhysicalDevice& physicalDevice, const uint32_t familyIndex)
        {
            return std::min<uint32_t>(physicalDevice.getQueueCount(familyIndex), 3);
        }
    }

    Device Renderer::createDevice(const Instance& instance, const PhysicalDevice& physicalDevice, const uint32_t graphicsFamilyIndex, const uint32_t queueCount)
    {
        std::vector<float> queuePriorities(queueCount, 1.0f);

        Device::DeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.familyIndex = graphicsFamilyIndex;
        deviceQueueCreateInfo.count = queueCount;
        deviceQueueCreateInfo.priorities = queuePriorities.data();

        const Device::CreateInfo deviceCreateInfo{ physicalDevice, 1, &deviceQueueCreateInfo };

        return Device{ instance, deviceCreateInfo };
    }

    Renderer::Renderer(platform::Window& window)
        : mWindow{ window }
        , mInstance{ Instance::CreateInfo{} }
        , mSurface{ mInstance, mWindow.get() }
        , mPhysicalDevice{ mInstance }
        , mGraphicsFamilyIndex{ computeGraphicsFamilyIndex(mPhysicalDevice, mSurface.get()) }
        , mQueueCount{ computeQueueCount(mPhysicalDevice, mGraphicsFamilyIndex) }
        , mDevice{ createDevice(mInstance, mPhysicalDevice, mGraphicsFamilyIndex, mQueueCount) }
        , mResourceManager{ mDevice, mPhysicalDevice }
        , mVkQueueGraphics{ getDeviceQueue(mDevice, mGraphicsFamilyIndex, 0) }
        , mVkQueueCompute{ getDeviceQueue(mDevice, mGraphicsFamilyIndex, std::min<uint32_t>(1, mQueueCount - 1)) }
        , mVkQueueCopy{ getDeviceQueue(mDevice, mGraphicsFamilyIndex, std::min<uint32_t>(2, mQueueCount - 1)) }
        , mSwapchain{ std::in_place, mDevice, Swapchain::CreateInfo{
            mPhysicalDevice,
            mSurface.get(),
            std::unordered_set<uint32_t>{ mGraphicsFamilyIndex },
            mWindow.getDescription().width,
            mWindow.getDescription().height } }
        , mGraphicsContextPool{ mDevice, CommandContext::CreateInfo{ mGraphicsFamilyIndex } }
        , mComputeContextPool{ mDevice, CommandContext::CreateInfo{ mGraphicsFamilyIndex } }
        , mRayTracingPass{ mDevice, mResourceManager }
        , mPresentPass{ mDevice, mResourceManager, mSwapchain->getFormat() }
        , mImGuiPass{ mInstance, mPhysicalDevice, mDevice, mVkQueueGraphics, mGraphicsFamilyIndex, mSwapchain->getFormat(), static_cast<uint32_t>(mSwapchain->getImageCount()), mWindow.get() }
        , mVkSemaphoresComputeFinished{}
        , mVkSemaphoresGraphicsReadDone{}
        , mHasGraphicsReadPending{}
        , mVkFencesGraphics{}
        , mVkFencesCompute{}
        , mVkFencesImagesInFlight{ mSwapchain->getImageCount(), VK_NULL_HANDLE }
        , mCurrentFrame{ 0 }
    {
        mResourceManager.registerImageView("BackBuffer", VK_NULL_HANDLE, mSwapchain->getExtent());

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            mVkSemaphoresComputeFinished[i] = createSemaphore(mDevice);
            mVkSemaphoresGraphicsReadDone[i] = createSemaphore(mDevice);
        }
    }

    Renderer::~Renderer()
    {
        mDevice.waitIdle();

        ShaderInput::clearPipelineLayoutCache();

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroySemaphore(mDevice.get(), mVkSemaphoresGraphicsReadDone[i], nullptr);
            vkDestroySemaphore(mDevice.get(), mVkSemaphoresComputeFinished[i], nullptr);
        }

        mSwapchain.reset();
    }

    void Renderer::update(const double /*timeStep*/)
    {
    }

    void Renderer::render(FrameConstants frame)
    {
        frame.index = mCurrentFrame % FRAMES_IN_FLIGHT;

        if (mVkFencesGraphics[frame.index] != VK_NULL_HANDLE)
        {
            const VkResult vkResultGraphicsWait = vkWaitForFences(mDevice.get(), 1, &mVkFencesGraphics[frame.index], VK_TRUE, UINT64_MAX);
            HEX_ASSERT(vkResultGraphicsWait == VK_SUCCESS, "Failed to wait for graphics VkFence. ", utility::translateErrorCode(vkResultGraphicsWait).c_str());
        }

        Swapchain::BackBuffer backBuffer;
        if (!mSwapchain->acquireNextBackBuffer(backBuffer))
        {
            const platform::Window::Description& description = mWindow.getDescription();
            resize(description.width, description.height);
            return;
        }
        const VkImage vkImageBackBuffer = backBuffer.image;
        mResourceManager.registerImageView("BackBuffer", backBuffer.view, backBuffer.extent);

        VkFence& vkFenceImageInFlight = mVkFencesImagesInFlight[backBuffer.imageIndex];
        if (vkFenceImageInFlight != VK_NULL_HANDLE)
        {
            const VkResult vkResultImageWait = vkWaitForFences(mDevice.get(), 1, &vkFenceImageInFlight, VK_TRUE, UINT64_MAX);
            HEX_ASSERT(vkResultImageWait == VK_SUCCESS, "Failed to wait for image-in-flight VkFence. ", utility::translateErrorCode(vkResultImageWait).c_str());
        }

        if (mVkFencesCompute[frame.index] != VK_NULL_HANDLE)
        {
            const VkResult vkResultComputeWait = vkWaitForFences(mDevice.get(), 1, &mVkFencesCompute[frame.index], VK_TRUE, UINT64_MAX);
            HEX_ASSERT(vkResultComputeWait == VK_SUCCESS, "Failed to wait for compute VkFence. ", utility::translateErrorCode(vkResultComputeWait).c_str());
        }

        CommandContext& computeContext = mComputeContextPool.requestContext();
        mVkFencesCompute[frame.index] = computeContext.getFence();
        computeContext.reset();

        mRayTracingPass.getInputOutputs(computeContext, mResourceManager, frame);
        mRayTracingPass.dispatch(computeContext, mResourceManager, frame);

        computeContext.finish();

        VkCommandBufferSubmitInfo vkCommandBufferSubmitInfoCompute{};
        vkCommandBufferSubmitInfoCompute.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        vkCommandBufferSubmitInfoCompute.commandBuffer = computeContext.get();

        VkSemaphoreSubmitInfo vkSemaphoreSubmitInfoWaitCompute{};
        vkSemaphoreSubmitInfoWaitCompute.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfoWaitCompute.semaphore = mVkSemaphoresGraphicsReadDone[frame.index];
        vkSemaphoreSubmitInfoWaitCompute.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

        VkSemaphoreSubmitInfo vkSemaphoreSubmitInfoSignalCompute{};
        vkSemaphoreSubmitInfoSignalCompute.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfoSignalCompute.semaphore = mVkSemaphoresComputeFinished[frame.index];
        vkSemaphoreSubmitInfoSignalCompute.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

        VkSubmitInfo2 vkSubmitInfo2Compute{};
        vkSubmitInfo2Compute.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        vkSubmitInfo2Compute.waitSemaphoreInfoCount = mHasGraphicsReadPending[frame.index] ? 1u : 0u;
        vkSubmitInfo2Compute.pWaitSemaphoreInfos = &vkSemaphoreSubmitInfoWaitCompute;
        vkSubmitInfo2Compute.commandBufferInfoCount = 1;
        vkSubmitInfo2Compute.pCommandBufferInfos = &vkCommandBufferSubmitInfoCompute;
        vkSubmitInfo2Compute.signalSemaphoreInfoCount = 1;
        vkSubmitInfo2Compute.pSignalSemaphoreInfos = &vkSemaphoreSubmitInfoSignalCompute;

        const VkResult vkResultComputeSubmit = vkQueueSubmit2(mVkQueueCompute, 1, &vkSubmitInfo2Compute, computeContext.getFence());
        HEX_ASSERT(vkResultComputeSubmit == VK_SUCCESS, "Failed to submit to compute VkQueue. ", utility::translateErrorCode(vkResultComputeSubmit).c_str());

        CommandContext& graphicsContext = mGraphicsContextPool.requestContext();
        mVkFencesGraphics[frame.index] = graphicsContext.getFence();
        vkFenceImageInFlight = graphicsContext.getFence();
        graphicsContext.reset();

        graphicsContext.transitionBarrier(vkImageBackBuffer, ResourceStates::Undefined, ResourceStates::RenderTarget);

        mPresentPass.getInputOutputs(graphicsContext, mResourceManager, frame);
        mPresentPass.render(graphicsContext, mResourceManager, frame);

        graphicsContext.transitionBarrier(vkImageBackBuffer, ResourceStates::RenderTarget, ResourceStates::RenderTarget);

        mImGuiPass.render(graphicsContext, mResourceManager, frame);

        graphicsContext.transitionBarrier(vkImageBackBuffer, ResourceStates::RenderTarget, ResourceStates::Present);

        graphicsContext.finish();

        VkCommandBufferSubmitInfo vkCommandBufferSubmitInfoGraphics{};
        vkCommandBufferSubmitInfoGraphics.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        vkCommandBufferSubmitInfoGraphics.commandBuffer = graphicsContext.get();

        std::array<VkSemaphoreSubmitInfo, 2> vkSemaphoreSubmitInfosWaitGraphics{};
        vkSemaphoreSubmitInfosWaitGraphics[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosWaitGraphics[0].semaphore = backBuffer.imageAvailable;
        vkSemaphoreSubmitInfosWaitGraphics[0].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        vkSemaphoreSubmitInfosWaitGraphics[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosWaitGraphics[1].semaphore = mVkSemaphoresComputeFinished[frame.index];
        vkSemaphoreSubmitInfosWaitGraphics[1].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

        std::array<VkSemaphoreSubmitInfo, 2> vkSemaphoreSubmitInfosSignalGraphics{};
        vkSemaphoreSubmitInfosSignalGraphics[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosSignalGraphics[0].semaphore = backBuffer.renderFinished;
        vkSemaphoreSubmitInfosSignalGraphics[0].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        vkSemaphoreSubmitInfosSignalGraphics[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosSignalGraphics[1].semaphore = mVkSemaphoresGraphicsReadDone[frame.index];
        vkSemaphoreSubmitInfosSignalGraphics[1].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

        VkSubmitInfo2 vkSubmitInfo2Graphics{};
        vkSubmitInfo2Graphics.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        vkSubmitInfo2Graphics.waitSemaphoreInfoCount = static_cast<uint32_t>(vkSemaphoreSubmitInfosWaitGraphics.size());
        vkSubmitInfo2Graphics.pWaitSemaphoreInfos = vkSemaphoreSubmitInfosWaitGraphics.data();
        vkSubmitInfo2Graphics.commandBufferInfoCount = 1;
        vkSubmitInfo2Graphics.pCommandBufferInfos = &vkCommandBufferSubmitInfoGraphics;
        vkSubmitInfo2Graphics.signalSemaphoreInfoCount = static_cast<uint32_t>(vkSemaphoreSubmitInfosSignalGraphics.size());
        vkSubmitInfo2Graphics.pSignalSemaphoreInfos = vkSemaphoreSubmitInfosSignalGraphics.data();

        const VkResult vkResultGraphicsSubmit = vkQueueSubmit2(mVkQueueGraphics, 1, &vkSubmitInfo2Graphics, graphicsContext.getFence());
        HEX_ASSERT(vkResultGraphicsSubmit == VK_SUCCESS, "Failed to submit to graphics VkQueue. ", utility::translateErrorCode(vkResultGraphicsSubmit).c_str());

        mHasGraphicsReadPending[frame.index] = true;

        if (!mSwapchain->present(mVkQueueGraphics, backBuffer))
        {
            const platform::Window::Description& description = mWindow.getDescription();
            resize(description.width, description.height);
        }

        ++mCurrentFrame;
    }

    void Renderer::resize(const uint32_t width, const uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            return;
        }

        mDevice.waitIdle();

        mSwapchain->resize(width, height);
        mVkFencesImagesInFlight.assign(mSwapchain->getImageCount(), VK_NULL_HANDLE);

        mRayTracingPass.resize(width, height, mResourceManager);
    }
}
