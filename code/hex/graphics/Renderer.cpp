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

        QueueFamilies computeQueueFamilies(const PhysicalDevice& physicalDevice, VkSurfaceKHR vkSurfaceKHR)
        {
            QueueFamilies queueFamilies{};

            queueFamilies.graphics = physicalDevice.getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
            HEX_ASSERT(physicalDevice.checkSurfaceSupport(queueFamilies.graphics, vkSurfaceKHR), "Graphics queue family does not support presentation to the given surface.");

            queueFamilies.compute = physicalDevice.getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
            if (queueFamilies.compute == UINT32_MAX)
            {
                queueFamilies.compute = physicalDevice.getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
            }

            queueFamilies.copy = physicalDevice.getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
            if (queueFamilies.copy == UINT32_MAX)
            {
                queueFamilies.copy = physicalDevice.getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
            }

            return queueFamilies;
        }

        void submitBatch(VkQueue vkQueue, const std::vector<VkCommandBufferSubmitInfo>& commandBufferSubmitInfos, const std::vector<VkSemaphoreSubmitInfo>& waitSemaphoreSubmitInfos, const std::vector<VkSemaphoreSubmitInfo>& signalSemaphoreSubmitInfos, VkFence vkFence)
        {
            VkSubmitInfo2 vkSubmitInfo2{};
            vkSubmitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
            vkSubmitInfo2.waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphoreSubmitInfos.size());
            vkSubmitInfo2.pWaitSemaphoreInfos = waitSemaphoreSubmitInfos.data();
            vkSubmitInfo2.commandBufferInfoCount = static_cast<uint32_t>(commandBufferSubmitInfos.size());
            vkSubmitInfo2.pCommandBufferInfos = commandBufferSubmitInfos.data();
            vkSubmitInfo2.signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphoreSubmitInfos.size());
            vkSubmitInfo2.pSignalSemaphoreInfos = signalSemaphoreSubmitInfos.data();

            const VkResult vkResult = vkQueueSubmit2(vkQueue, 1, &vkSubmitInfo2, vkFence);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to submit to VkQueue. ", utility::translateErrorCode(vkResult).c_str());
        }
    }

    Device Renderer::createDevice(const Instance& instance, const PhysicalDevice& physicalDevice, const QueueFamilies& queueFamilies)
    {
        const std::set<uint32_t> distinctFamilies{ queueFamilies.graphics, queueFamilies.compute, queueFamilies.copy };

        float queuePriority = 1.0f;
        std::vector<Device::DeviceQueueCreateInfo> deviceQueueCreateInfos;
        for (const uint32_t familyIndex : distinctFamilies)
        {
            Device::DeviceQueueCreateInfo deviceQueueCreateInfo{};
            deviceQueueCreateInfo.familyIndex = familyIndex;
            deviceQueueCreateInfo.count = 1;
            deviceQueueCreateInfo.priorities = &queuePriority;
            deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
        }

        const Device::CreateInfo deviceCreateInfo{ physicalDevice, deviceQueueCreateInfos.size(), deviceQueueCreateInfos.data() };

        return Device{ instance, deviceCreateInfo };
    }

    Renderer::Renderer(platform::Window& window)
        : mWindow{ window }
        , mInstance{ Instance::CreateInfo{} }
        , mSurface{ mInstance, mWindow.get() }
        , mPhysicalDevice{ mInstance }
        , mQueueFamilies{ computeQueueFamilies(mPhysicalDevice, mSurface.get()) }
        , mDevice{ createDevice(mInstance, mPhysicalDevice, mQueueFamilies) }
        , mResourceManager{ mDevice, mPhysicalDevice }
        , mVkQueueGraphics{ getDeviceQueue(mDevice, mQueueFamilies.graphics, QUEUE_INDEX) }
        , mVkQueueCompute{ getDeviceQueue(mDevice, mQueueFamilies.compute, QUEUE_INDEX) }
        , mVkQueueCopy{ getDeviceQueue(mDevice, mQueueFamilies.copy, QUEUE_INDEX) }
        , mSwapchain{ std::in_place, mDevice, Swapchain::CreateInfo{
            mPhysicalDevice,
            mSurface.get(),
            std::unordered_set<uint32_t>{ mQueueFamilies.graphics },
            mWindow.getDescription().width,
            mWindow.getDescription().height } }
        , mGraphicsContextPool{ mDevice, CommandContext::CreateInfo{ mQueueFamilies.graphics } }
        , mComputeContextPool{ mDevice, CommandContext::CreateInfo{ mQueueFamilies.compute } }
        , mCopyContextPool{ mDevice, CommandContext::CreateInfo{ mQueueFamilies.copy } }
        , mRayTracingPass{ mDevice, mResourceManager }
        , mPresentPass{ mDevice, mResourceManager, mSwapchain->getFormat() }
        , mImGuiPass{ mInstance, mPhysicalDevice, mDevice, mVkQueueGraphics, mQueueFamilies.graphics, mSwapchain->getFormat(), static_cast<uint32_t>(mSwapchain->getImageCount()), mWindow.get() }
        , mVkSemaphoresComputeFinished{}
        , mVkSemaphoresGraphicsReadDone{}
        , mVkSemaphoresCopyFinished{}
        , mHasGraphicsReadPending{}
        , mHasCopyPending{}
        , mVkFencesGraphics{}
        , mVkFencesCompute{}
        , mVkFencesCopy{}
        , mVkFencesImagesInFlight{ mSwapchain->getImageCount(), VK_NULL_HANDLE }
        , mCurrentFrame{ 0 }
    {
        mResourceManager.registerImageView("RT_BackBuffer_Renderer", VK_NULL_HANDLE, mSwapchain->getExtent());

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            mVkSemaphoresComputeFinished[i] = createSemaphore(mDevice);
            mVkSemaphoresGraphicsReadDone[i] = createSemaphore(mDevice);
            mVkSemaphoresCopyFinished[i] = createSemaphore(mDevice);
        }
    }

    Renderer::~Renderer()
    {
        mDevice.waitIdle();

        mRayTracingPass.freeGPUUploadData(mResourceManager);

        ShaderInput::clearPipelineLayoutCache();

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroySemaphore(mDevice.get(), mVkSemaphoresCopyFinished[i], nullptr);
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

        waitForFence(mVkFencesGraphics[frame.index]);

        Swapchain::BackBuffer backBuffer;
        if (!acquireBackBuffer(backBuffer))
        {
            return;
        }

        waitForFence(mVkFencesImagesInFlight[backBuffer.imageIndex]);
        waitForFence(mVkFencesCompute[frame.index]);
        waitForFence(mVkFencesCopy[frame.index]);

        if (mRayTracingPass.needsCopyDataToGPU())
        {
            CommandContext& copyContext = mCopyContextPool.requestContext();
            mVkFencesCopy[frame.index] = copyContext.getFence();
            copyContext.reset();

            mRayTracingPass.copyDataToGPU(copyContext, mQueueFamilies, mResourceManager);

            submitCopy(frame, { copyContext.finish() });
        }

        CommandContext& rayTracingPassComputeContext = mComputeContextPool.requestContext();
        mVkFencesCompute[frame.index] = rayTracingPassComputeContext.getFence();
        rayTracingPassComputeContext.reset();

        mRayTracingPass.preDispatch(rayTracingPassComputeContext, mQueueFamilies, mResourceManager, frame);
        mRayTracingPass.dispatch(rayTracingPassComputeContext, mResourceManager, frame);
        mRayTracingPass.postDispatch(rayTracingPassComputeContext, mQueueFamilies, mResourceManager, frame);

        submitCompute(frame, { rayTracingPassComputeContext.finish() });

        CommandContext& presentPassGraphicsContext = mGraphicsContextPool.requestContext();
        mVkFencesGraphics[frame.index] = presentPassGraphicsContext.getFence();
        mVkFencesImagesInFlight[backBuffer.imageIndex] = presentPassGraphicsContext.getFence();
        presentPassGraphicsContext.reset();

        presentPassGraphicsContext.transitionBarrier(backBuffer.image, ResourceStates::Undefined, ResourceStates::RenderTarget);

        mPresentPass.preRender(presentPassGraphicsContext, mQueueFamilies, mResourceManager, frame);
        mPresentPass.render(presentPassGraphicsContext, mResourceManager, frame);
        mPresentPass.postRender(presentPassGraphicsContext, mQueueFamilies, mResourceManager, frame);

        presentPassGraphicsContext.transitionBarrier(backBuffer.image, ResourceStates::RenderTarget, ResourceStates::RenderTarget);

        mImGuiPass.preRender(presentPassGraphicsContext, mQueueFamilies, mResourceManager, frame);
        mImGuiPass.render(presentPassGraphicsContext, mResourceManager, frame);
        mImGuiPass.postRender(presentPassGraphicsContext, mQueueFamilies, mResourceManager, frame);

        presentPassGraphicsContext.transitionBarrier(backBuffer.image, ResourceStates::RenderTarget, ResourceStates::Present);

        submitGraphics(frame, backBuffer, { presentPassGraphicsContext.finish() });

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

    void Renderer::waitForFence(VkFence fence) const
    {
        if (fence == VK_NULL_HANDLE)
        {
            return;
        }

        const VkResult vkResult = vkWaitForFences(mDevice.get(), 1, &fence, VK_TRUE, UINT64_MAX);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to wait for VkFence. ", utility::translateErrorCode(vkResult).c_str());
    }

    bool Renderer::acquireBackBuffer(Swapchain::BackBuffer& backBuffer)
    {
        if (!mSwapchain->acquireNextBackBuffer(backBuffer))
        {
            const platform::Window::Description& description = mWindow.getDescription();
            resize(description.width, description.height);
            return false;
        }

        mResourceManager.registerImageView("RT_BackBuffer_Renderer", backBuffer.view, backBuffer.extent);
        return true;
    }

    void Renderer::submitCopy(FrameConstants frame, const std::vector<VkCommandBufferSubmitInfo>& commandBufferSubmitInfos)
    {
        std::array<VkSemaphoreSubmitInfo, FRAMES_IN_FLIGHT> vkSemaphoreSubmitInfosSignalCopy{};
        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
        {
            vkSemaphoreSubmitInfosSignalCopy[i].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            vkSemaphoreSubmitInfosSignalCopy[i].semaphore = mVkSemaphoresCopyFinished[i];
            vkSemaphoreSubmitInfosSignalCopy[i].stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            mHasCopyPending[i] = true;
        }

        submitBatch(mVkQueueCopy, commandBufferSubmitInfos, {}, { vkSemaphoreSubmitInfosSignalCopy.begin(), vkSemaphoreSubmitInfosSignalCopy.end() }, mVkFencesCopy[frame.index]);
    }

    void Renderer::submitCompute(FrameConstants frame, const std::vector<VkCommandBufferSubmitInfo>& commandBufferSubmitInfos)
    {
        std::vector<VkSemaphoreSubmitInfo> vkSemaphoreSubmitInfosWaitCompute;
        if (mHasGraphicsReadPending[frame.index])
        {
            VkSemaphoreSubmitInfo vkSemaphoreSubmitInfo{};
            vkSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            vkSemaphoreSubmitInfo.semaphore = mVkSemaphoresGraphicsReadDone[frame.index];
            vkSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            vkSemaphoreSubmitInfosWaitCompute.push_back(vkSemaphoreSubmitInfo);
        }
        if (mHasCopyPending[frame.index])
        {
            VkSemaphoreSubmitInfo vkSemaphoreSubmitInfo{};
            vkSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            vkSemaphoreSubmitInfo.semaphore = mVkSemaphoresCopyFinished[frame.index];
            vkSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            vkSemaphoreSubmitInfosWaitCompute.push_back(vkSemaphoreSubmitInfo);
            mHasCopyPending[frame.index] = false;
        }

        VkSemaphoreSubmitInfo vkSemaphoreSubmitInfoSignalCompute{};
        vkSemaphoreSubmitInfoSignalCompute.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfoSignalCompute.semaphore = mVkSemaphoresComputeFinished[frame.index];
        vkSemaphoreSubmitInfoSignalCompute.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

        submitBatch(mVkQueueCompute, commandBufferSubmitInfos, vkSemaphoreSubmitInfosWaitCompute, { vkSemaphoreSubmitInfoSignalCompute }, mVkFencesCompute[frame.index]);
    }

    void Renderer::submitGraphics(FrameConstants frame, const Swapchain::BackBuffer& backBuffer, const std::vector<VkCommandBufferSubmitInfo>& commandBufferSubmitInfos)
    {
        std::vector<VkSemaphoreSubmitInfo> vkSemaphoreSubmitInfosWaitGraphics(2);
        vkSemaphoreSubmitInfosWaitGraphics[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosWaitGraphics[0].semaphore = backBuffer.imageAvailable;
        vkSemaphoreSubmitInfosWaitGraphics[0].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        vkSemaphoreSubmitInfosWaitGraphics[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosWaitGraphics[1].semaphore = mVkSemaphoresComputeFinished[frame.index];
        vkSemaphoreSubmitInfosWaitGraphics[1].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

        std::vector<VkSemaphoreSubmitInfo> vkSemaphoreSubmitInfosSignalGraphics(2);
        vkSemaphoreSubmitInfosSignalGraphics[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosSignalGraphics[0].semaphore = backBuffer.renderFinished;
        vkSemaphoreSubmitInfosSignalGraphics[0].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        vkSemaphoreSubmitInfosSignalGraphics[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        vkSemaphoreSubmitInfosSignalGraphics[1].semaphore = mVkSemaphoresGraphicsReadDone[frame.index];
        vkSemaphoreSubmitInfosSignalGraphics[1].stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

        submitBatch(mVkQueueGraphics, commandBufferSubmitInfos, vkSemaphoreSubmitInfosWaitGraphics, vkSemaphoreSubmitInfosSignalGraphics, mVkFencesGraphics[frame.index]);

        mHasGraphicsReadPending[frame.index] = true;
    }
}
