#pragma once

#include "vkCommon.h"

#include "resourceState.h"

namespace graphics
{
    class Device;
    class Pipeline;

    class CommandContext
    {
    public:
        struct CreateInfo
        {
            uint32_t queueFamilyIndex = 0;
        };

        struct RenderingInfo
        {
            VkImageView vkImageViewColor = VK_NULL_HANDLE;
            VkRect2D vkRect2DExtent = {{0, 0}, {0, 0}};
            std::array<float, 4> clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            bool clear = true;
            VkImageView vkImageViewDepth = VK_NULL_HANDLE;
            float clearDepth = 1.0f;
        };

        CommandContext(const Device& device, const CreateInfo& createInfo);
        ~CommandContext();

        CommandContext(const CommandContext&) = delete;
        CommandContext& operator=(const CommandContext&) = delete;

        CommandContext(CommandContext&&) = delete;
        CommandContext& operator=(CommandContext&&) = delete;

        inline const Device& getDevice() const { return mDevice; }
        inline VkCommandBuffer get() const { return mVkCommandBuffer; }
        inline VkFence getFence() const { return mVkFence; }

        void beginRendering(const RenderingInfo& renderingInfo);
        void endRendering();

        void transitionBarrier(const VkBuffer& vkBuffer, const ResourceState& vkResourceStateOld, const ResourceState& vkResourceStateNew, const uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, const uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, const uint64_t size = VK_WHOLE_SIZE, const uint64_t offset = 0);
        void transitionBarrier(const VkImage& vkImage, const ResourceState& vkResourceStateOld, const ResourceState& vkResourceStateNew, const VkImageAspectFlags vkImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, const uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, const uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED);

        void copyBuffer(const VkBuffer& vkBufferSrc, const VkBuffer& vkBufferDst, const uint64_t size, const uint64_t srcOffset = 0, const uint64_t dstOffset = 0);
        void copyBufferToImage(const VkBuffer& vkBufferSrc, VkImage& vkImageDst, const uint32_t width, const uint32_t height, const uint32_t depth = 1);

        void setViewport(const float x, const float y, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f);
        void setScissor(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height);

        void bindPipeline(const Pipeline& pipeline);
        void bindDescriptorSets(VkPipelineLayout vkPipelineLayout, const uint32_t firstSet, const std::vector<VkDescriptorSet>& vkDescriptorSets);
        void pushConstants(VkPipelineLayout vkPipelineLayout, const VkShaderStageFlags vkShaderStageFlags, const uint32_t offset, const uint32_t size, const void* data);

        void bindVertexBuffer(const VkBuffer& vkBuffer, const uint32_t binding = 0, const uint64_t offset = 0);
        void bindIndexBuffer(const VkBuffer& vkBuffer, const bool use16BitIndices = false, const uint64_t offset = 0);

        void draw(const uint32_t vertexCount, const uint32_t instanceCount = 1, const uint32_t firstVertex = 0, const uint32_t firstInstance = 0);
        void drawIndexed(const uint32_t indexCount, const uint32_t instanceCount = 1, const uint32_t firstIndex = 0, const int32_t vertexOffset = 0, const uint32_t firstInstance = 0);
        void dispatch(const uint32_t groupCountX, const uint32_t groupCountY = 1, const uint32_t groupCountZ = 1);
        void dispatch1D(const uint32_t threadCountX, const uint32_t groupSizeX = 64);
        void dispatch2D(const uint32_t threadCountX, const uint32_t threadCountY, const uint32_t groupSizeX = 8, const uint32_t groupSizeY = 8);
        void dispatch3D(const uint32_t threadCountX, const uint32_t threadCountY, const uint32_t threadCountZ, const uint32_t groupSizeX, const uint32_t groupSizeY, const uint32_t groupSizeZ);

        void reset();
        VkCommandBufferSubmitInfo finish();

    private:
        const Device& mDevice;

        VkCommandPool mVkCommandPool;
        VkCommandBuffer mVkCommandBuffer;
        VkFence mVkFence;

        VkPipelineBindPoint mVkPipelineBindPoint;
    };
}
