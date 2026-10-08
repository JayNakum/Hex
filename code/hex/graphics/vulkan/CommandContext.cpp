#include "CommandContext.h"

#include "Device.h"
#include "Pipeline.h"

namespace graphics
{
    namespace
    {
        VkCommandPool createCommandPool(const Device& device, const uint32_t queueFamilyIndex)
        {
            VkCommandPoolCreateInfo vkCommandPoolCreateInfo{};
            vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            vkCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            vkCommandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

            VkCommandPool vkCommandPool = VK_NULL_HANDLE;
            const VkResult vkResult = vkCreateCommandPool(device.get(), &vkCommandPoolCreateInfo, nullptr, &vkCommandPool);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkCommandPool. ", utility::translateErrorCode(vkResult).c_str());

            return vkCommandPool;
        }

        VkCommandBuffer allocateCommandBuffer(const Device& device, VkCommandPool vkCommandPool)
        {
            VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{};
            vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            vkCommandBufferAllocateInfo.commandPool = vkCommandPool;
            vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            vkCommandBufferAllocateInfo.commandBufferCount = 1;

            VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
            const VkResult vkResult = vkAllocateCommandBuffers(device.get(), &vkCommandBufferAllocateInfo, &vkCommandBuffer);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to allocate VkCommandBuffer. ", utility::translateErrorCode(vkResult).c_str());

            return vkCommandBuffer;
        }

        VkFence createSignaledFence(const Device& device)
        {
            VkFenceCreateInfo vkFenceCreateInfo{};
            vkFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            vkFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            VkFence vkFence = VK_NULL_HANDLE;
            const VkResult vkResult = vkCreateFence(device.get(), &vkFenceCreateInfo, nullptr, &vkFence);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkFence. ", utility::translateErrorCode(vkResult).c_str());

            return vkFence;
        }
    }

    CommandContext::CommandContext(const Device& device, const CreateInfo& createInfo)
        : mDevice{ device }
        , mVkCommandPool{ createCommandPool(device, createInfo.queueFamilyIndex) }
        , mVkCommandBuffer{ allocateCommandBuffer(device, mVkCommandPool) }
        , mVkFence{ createSignaledFence(device) }
        , mVkPipelineBindPoint{ VK_PIPELINE_BIND_POINT_GRAPHICS }
    {
    }

    CommandContext::~CommandContext()
    {
        vkDestroyFence(mDevice.get(), mVkFence, nullptr);
        vkDestroyCommandPool(mDevice.get(), mVkCommandPool, nullptr);
        mVkCommandPool = VK_NULL_HANDLE;
        mVkCommandBuffer = VK_NULL_HANDLE;
        mVkFence = VK_NULL_HANDLE;
    }

    void CommandContext::beginRendering(const RenderingInfo& renderingInfo)
    {
        VkRenderingAttachmentInfo vkRenderingAttachmentInfoColor{};
        vkRenderingAttachmentInfoColor.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        vkRenderingAttachmentInfoColor.imageView = renderingInfo.vkImageViewColor;
        vkRenderingAttachmentInfoColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        vkRenderingAttachmentInfoColor.loadOp = renderingInfo.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        vkRenderingAttachmentInfoColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        vkRenderingAttachmentInfoColor.clearValue.color.float32[0] = renderingInfo.clearColor[0];
        vkRenderingAttachmentInfoColor.clearValue.color.float32[1] = renderingInfo.clearColor[1];
        vkRenderingAttachmentInfoColor.clearValue.color.float32[2] = renderingInfo.clearColor[2];
        vkRenderingAttachmentInfoColor.clearValue.color.float32[3] = renderingInfo.clearColor[3];

        VkRenderingAttachmentInfo vkRenderingAttachmentInfoDepth{};
        if (renderingInfo.vkImageViewDepth != VK_NULL_HANDLE)
        {
            vkRenderingAttachmentInfoDepth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            vkRenderingAttachmentInfoDepth.imageView = renderingInfo.vkImageViewDepth;
            vkRenderingAttachmentInfoDepth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            vkRenderingAttachmentInfoDepth.loadOp = renderingInfo.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            vkRenderingAttachmentInfoDepth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            vkRenderingAttachmentInfoDepth.clearValue.depthStencil = { renderingInfo.clearDepth, 0 };
        }

        VkRenderingInfo vkRenderingInfo{};
        vkRenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        vkRenderingInfo.renderArea = renderingInfo.vkRect2DExtent;
        vkRenderingInfo.layerCount = 1;
        vkRenderingInfo.colorAttachmentCount = 1;
        vkRenderingInfo.pColorAttachments = &vkRenderingAttachmentInfoColor;
        vkRenderingInfo.pDepthAttachment = renderingInfo.vkImageViewDepth != VK_NULL_HANDLE ? &vkRenderingAttachmentInfoDepth : nullptr;
        vkCmdBeginRendering(mVkCommandBuffer, &vkRenderingInfo);
    }

    void CommandContext::endRendering()
    {
        vkCmdEndRendering(mVkCommandBuffer);
    }

    void CommandContext::transitionBarrier(const VkBuffer& vkBuffer, const ResourceState& vkResourceStateOld, const ResourceState& vkResourceStateNew, const uint32_t srcQueueFamilyIndex, const uint32_t dstQueueFamilyIndex, const uint64_t size, const uint64_t offset)
    {
        VkBufferMemoryBarrier2 vkBufferMemoryBarrier2{};
        vkBufferMemoryBarrier2.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
        vkBufferMemoryBarrier2.srcStageMask = vkResourceStateOld.vkPipelineStageFlags;
        vkBufferMemoryBarrier2.srcAccessMask = vkResourceStateOld.vkAccessFlags;
        vkBufferMemoryBarrier2.dstStageMask = vkResourceStateNew.vkPipelineStageFlags;
        vkBufferMemoryBarrier2.dstAccessMask = vkResourceStateNew.vkAccessFlags;
        vkBufferMemoryBarrier2.srcQueueFamilyIndex = srcQueueFamilyIndex;
        vkBufferMemoryBarrier2.dstQueueFamilyIndex = dstQueueFamilyIndex;
        vkBufferMemoryBarrier2.buffer = vkBuffer;
        vkBufferMemoryBarrier2.offset = offset;
        vkBufferMemoryBarrier2.size = size;

        VkDependencyInfo vkDependencyInfo{};
        vkDependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        vkDependencyInfo.bufferMemoryBarrierCount = 1;
        vkDependencyInfo.pBufferMemoryBarriers = &vkBufferMemoryBarrier2;

        vkCmdPipelineBarrier2(mVkCommandBuffer, &vkDependencyInfo);
    }

    void CommandContext::transitionBarrier(const VkImage& vkImage, const ResourceState& vkResourceStateOld, const ResourceState& vkResourceStateNew, const VkImageAspectFlags vkImageAspectFlags, const uint32_t srcQueueFamilyIndex, const uint32_t dstQueueFamilyIndex)
    {
        VkImageMemoryBarrier2 vkImageMemoryBarrier2{};
        vkImageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        vkImageMemoryBarrier2.srcStageMask = vkResourceStateOld.vkPipelineStageFlags;
        vkImageMemoryBarrier2.srcAccessMask = vkResourceStateOld.vkAccessFlags;
        vkImageMemoryBarrier2.dstStageMask = vkResourceStateNew.vkPipelineStageFlags;
        vkImageMemoryBarrier2.dstAccessMask = vkResourceStateNew.vkAccessFlags;
        vkImageMemoryBarrier2.oldLayout = vkResourceStateOld.vkImageLayout;
        vkImageMemoryBarrier2.newLayout = vkResourceStateNew.vkImageLayout;
        vkImageMemoryBarrier2.srcQueueFamilyIndex = srcQueueFamilyIndex;
        vkImageMemoryBarrier2.dstQueueFamilyIndex = dstQueueFamilyIndex;
        vkImageMemoryBarrier2.image = vkImage;
        vkImageMemoryBarrier2.subresourceRange = { vkImageAspectFlags, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS };

        VkDependencyInfo vkDependencyInfo{};
        vkDependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        vkDependencyInfo.imageMemoryBarrierCount = 1;
        vkDependencyInfo.pImageMemoryBarriers = &vkImageMemoryBarrier2;

        vkCmdPipelineBarrier2(mVkCommandBuffer, &vkDependencyInfo);
    }

    void CommandContext::copyBuffer(const VkBuffer& vkBufferSrc, const VkBuffer& vkBufferDst, const uint64_t size, const uint64_t srcOffset, const uint64_t dstOffset)
    {
        VkBufferCopy vkBufferCopyRegion{};
        vkBufferCopyRegion.srcOffset = srcOffset;
        vkBufferCopyRegion.dstOffset = dstOffset;
        vkBufferCopyRegion.size = size;
        vkCmdCopyBuffer(mVkCommandBuffer, vkBufferSrc, vkBufferDst, 1, &vkBufferCopyRegion);
    }

    void CommandContext::copyBufferToImage(const VkBuffer& vkBufferSrc, VkImage& vkImageDst, const uint32_t width, const uint32_t height, const uint32_t depth)
    {
        VkBufferImageCopy vkBufferImageCopyRegion{};
        vkBufferImageCopyRegion.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        vkBufferImageCopyRegion.imageExtent = { width, height, depth };
        vkCmdCopyBufferToImage(mVkCommandBuffer, vkBufferSrc, vkImageDst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferImageCopyRegion);
    }

    void CommandContext::setViewport(const float x, const float y, const float width, const float height, const float minDepth, const float maxDepth)
    {
        VkViewport vkViewport{ x, y, width, height, minDepth, maxDepth };
        vkCmdSetViewport(mVkCommandBuffer, 0, 1, &vkViewport);
    }

    void CommandContext::setScissor(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height)
    {
        VkRect2D vkRect2DScissor{ { x, y }, { width, height } };
        vkCmdSetScissor(mVkCommandBuffer, 0, 1, &vkRect2DScissor);
    }

    void CommandContext::bindPipeline(const Pipeline& pipeline)
    {
        mVkPipelineBindPoint = pipeline.getBindPoint();
        vkCmdBindPipeline(mVkCommandBuffer, mVkPipelineBindPoint, pipeline.get());
    }

    void CommandContext::bindDescriptorSets(VkPipelineLayout vkPipelineLayout, const uint32_t firstSet, const std::vector<VkDescriptorSet>& vkDescriptorSets)
    {
        vkCmdBindDescriptorSets(mVkCommandBuffer, mVkPipelineBindPoint, vkPipelineLayout, firstSet, static_cast<uint32_t>(vkDescriptorSets.size()), vkDescriptorSets.data(), 0, nullptr);
    }

    void CommandContext::pushConstants(VkPipelineLayout vkPipelineLayout, const VkShaderStageFlags vkShaderStageFlags, const uint32_t offset, const uint32_t size, const void* data)
    {
        vkCmdPushConstants(mVkCommandBuffer, vkPipelineLayout, vkShaderStageFlags, offset, size, data);
    }

    void CommandContext::bindVertexBuffer(const VkBuffer& vkBuffer, const uint32_t binding, const uint64_t offset)
    {
        const VkDeviceSize vkDeviceSizeOffset = static_cast<VkDeviceSize>(offset);
        vkCmdBindVertexBuffers(mVkCommandBuffer, binding, 1, &vkBuffer, &vkDeviceSizeOffset);
    }

    void CommandContext::bindIndexBuffer(const VkBuffer& vkBuffer, const bool use16BitIndices, const uint64_t offset)
    {
        vkCmdBindIndexBuffer(mVkCommandBuffer, vkBuffer, static_cast<VkDeviceSize>(offset), use16BitIndices ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
    }

    void CommandContext::draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
    {
        vkCmdDraw(mVkCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandContext::drawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance)
    {
        vkCmdDrawIndexed(mVkCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandContext::dispatch(const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ)
    {
        vkCmdDispatch(mVkCommandBuffer, groupCountX, groupCountY, groupCountZ);
    }

    void CommandContext::dispatch1D(const uint32_t threadCountX, const uint32_t groupSizeX)
    {
        dispatch(maths::divideByMultiple(threadCountX, groupSizeX), 1, 1);
    }

    void CommandContext::dispatch2D(const uint32_t threadCountX, const uint32_t threadCountY, const uint32_t groupSizeX, const uint32_t groupSizeY)
    {
        dispatch(maths::divideByMultiple(threadCountX, groupSizeX), maths::divideByMultiple(threadCountY, groupSizeY), 1);
    }

    void CommandContext::dispatch3D(const uint32_t threadCountX, const uint32_t threadCountY, const uint32_t threadCountZ, const uint32_t groupSizeX, const uint32_t groupSizeY, const uint32_t groupSizeZ)
    {
        dispatch(maths::divideByMultiple(threadCountX, groupSizeX), maths::divideByMultiple(threadCountY, groupSizeY), maths::divideByMultiple(threadCountZ, groupSizeZ));
    }

    void CommandContext::reset()
    {
        const VkResult vkResultFenceReset = vkResetFences(mDevice.get(), 1, &mVkFence);
        HEX_ASSERT(vkResultFenceReset == VK_SUCCESS, "Failed to reset VkFence. ", utility::translateErrorCode(vkResultFenceReset).c_str());

        const VkResult vkResultReset = vkResetCommandBuffer(mVkCommandBuffer, 0);
        HEX_ASSERT(vkResultReset == VK_SUCCESS, "Failed to reset VkCommandBuffer. ", utility::translateErrorCode(vkResultReset).c_str());

        VkCommandBufferBeginInfo vkCommandBufferBeginInfo{};
        vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        const VkResult vkResultBegin = vkBeginCommandBuffer(mVkCommandBuffer, &vkCommandBufferBeginInfo);
        HEX_ASSERT(vkResultBegin == VK_SUCCESS, "Failed to begin VkCommandBuffer. ", utility::translateErrorCode(vkResultBegin).c_str());

        mVkPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    }

    VkCommandBufferSubmitInfo CommandContext::finish()
    {
        const VkResult vkResultEnd = vkEndCommandBuffer(mVkCommandBuffer);
        HEX_ASSERT(vkResultEnd == VK_SUCCESS, "Failed to end VkCommandBuffer. ", utility::translateErrorCode(vkResultEnd).c_str());

        VkCommandBufferSubmitInfo vkCommandBufferSubmitInfo{};
        vkCommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        vkCommandBufferSubmitInfo.commandBuffer = mVkCommandBuffer;
        return vkCommandBufferSubmitInfo;
    }
}
