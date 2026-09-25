#pragma once

#include "core/common.h"

#include "vulkan/MemoryHeap.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/ResourceTable.h"
#include "vulkan/resourceState.h"

namespace graphics
{
    class Device;
    class PhysicalDevice;
    class CommandContext;
}

namespace graphics
{
    class ResourceManager
    {
    public:
        struct BufferCreateInfo
        {
            uint64_t byteSize = 0;
            VkBufferUsageFlags usage = 0;
        };

        struct ImageViewCreateInfo
        {
            VkFormat vkFormat = VK_FORMAT_UNDEFINED;
            VkImageAspectFlags vkImageAspectFlags = 0;
            uint32_t baseMipLevel = 0;
            uint32_t levelCount = 1;
            uint32_t baseArrayLayer = 0;
            uint32_t layerCount = 1;
        };

        struct SamplerCreateInfo
        {
            VkFilter vkMagFilter = VK_FILTER_LINEAR;
            VkFilter vkMinFilter = VK_FILTER_LINEAR;
            VkSamplerMipmapMode vkMipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            VkSamplerAddressMode vkAddressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            VkSamplerAddressMode vkAddressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            VkSamplerAddressMode vkAddressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            float maxAnisotropy = 1.0f;
        };

        ResourceManager(const Device& device, const PhysicalDevice& physicalDevice);
        ~ResourceManager();

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        const VkImage& createRenderTexture(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage, const MemoryHeap::ResourceCategory category);
        const VkImage& createDepthTexture(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format = VK_FORMAT_D32_SFLOAT, const MemoryHeap::ResourceCategory category = MemoryHeap::ResourceCategory::RenderTargetData);
        const VkImage& recreateRenderTexture(const std::string& name, const uint32_t width, const uint32_t height);

        VkBuffer& createBuffer(const std::string& name, const BufferCreateInfo& createInfo, const MemoryHeap::ResourceCategory category);

        VkImage& getImage(const std::string& name);
        const VkImage& getImage(const std::string& name) const;
        VkExtent2D getExtent(const std::string& name) const;
        VkBuffer& getBuffer(const std::string& name);
        const VkBuffer& getBuffer(const std::string& name) const;
        void writeBuffer(const std::string& name, const void* data, const uint64_t size, const uint64_t offset = 0) const;

        void transitionImage(CommandContext& context, const std::string& name, const ResourceState& newState);

        VkImageView createImageView(const std::string& name, const ImageViewCreateInfo& viewInfo, const std::string& alias);
        VkImageView getImageView(const std::string& name, const std::string& alias = "") const;
        void destroyImageView(const std::string& name, const std::string& alias = "");

        void registerImageView(const std::string& name, VkImageView vkImageView, const VkExtent2D& vkExtent2D);

        ResourceTable& createResourceTable(const std::string& name, const ResourceTable::Layout& layout);
        ResourceTable& getResourceTable(const std::string& name);

        VkSampler createSampler(const std::string& name, const SamplerCreateInfo& createInfo);
        VkSampler getSampler(const std::string& name) const;

        inline MemoryHeap& memoryHeap() { return mMemoryHeap; }
        inline DescriptorPool& descriptorAllocator() { return mDescriptorPool; }

    private:
        struct ImageResource
        {
            VkImage vkImage;
            VkFormat vkFormat;
            VkExtent2D vkExtent2D;
            VkImageUsageFlags vkImageUsageFlags;
            VkImageAspectFlags vkImageAspectFlags;
            ResourceState currentState;
            MemoryHeap::ResourceCategory category;
            uint64_t allocationOffset;
            uint64_t allocationSize;
        };

        struct BufferResource
        {
            VkBuffer vkBuffer;
            MemoryHeap::ResourceCategory category;
            uint64_t allocationOffset;
            uint64_t allocationSize;
        };

        const VkImage& createImageResource(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage, const MemoryHeap::ResourceCategory category, const VkImageAspectFlags aspect);
        static VkImageView buildImageView(const Device& device, const ImageResource& imageResource, const ImageViewCreateInfo& viewInfo);
        static std::string buildImageViewKey(const std::string& name, const std::string& alias);
        void destroyViewsForResource(const std::string& name);

    private:
        const Device& mDevice;

        MemoryHeap mMemoryHeap;
        DescriptorPool mDescriptorPool;

        std::unordered_map<std::string, ImageResource> mImages;
        std::unordered_map<std::string, VkImageView> mImageViews;
        std::unordered_map<std::string, BufferResource> mBuffers;
        std::unordered_map<std::string, ResourceTable> mResourceTables;
        std::unordered_map<std::string, VkSampler> mSamplers;
        std::unordered_map<std::string, VkImageView> mRegisteredImageViews;
        std::unordered_map<std::string, VkExtent2D> mRegisteredExtents;
    };
}
