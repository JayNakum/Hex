#pragma once

#include "core/common.h"

#include "vulkan/MemoryHeap.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/ResourceTable.h"

namespace graphics
{
    class Device;
    class PhysicalDevice;
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

        // Images
        const VkImage& createRenderTexture(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage, const MemoryHeap::ResourceCategory category);
        const VkImage& createDepthTexture(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format = VK_FORMAT_D32_SFLOAT, const MemoryHeap::ResourceCategory category = MemoryHeap::ResourceCategory::RenderTargetData);
        const VkImage& recreateRenderTexture(const std::string& name, const uint32_t width, const uint32_t height);
        void releaseImage(const std::string& name);

        VkImage& getImage(const std::string& name);
        const VkImage& getImage(const std::string& name) const;
        VkExtent2D getExtent(const std::string& name) const;

        VkImageView createImageView(const std::string& name, const ImageViewCreateInfo& viewInfo, const std::string& alias);
        VkImageView getImageView(const std::string& name, const std::string& alias = "") const;
        void destroyImageView(const std::string& name, const std::string& alias = "");

        void registerImageView(const std::string& name, VkImageView vkImageView, const VkExtent2D& vkExtent2D);

        // Buffers
        VkBuffer& createBuffer(const std::string& name, const BufferCreateInfo& createInfo, const MemoryHeap::ResourceCategory category);
        void releaseBuffer(const std::string& name);

        VkBuffer& getBuffer(const std::string& name);
        const VkBuffer& getBuffer(const std::string& name) const;
        void writeBuffer(const std::string& name, const void* data, const uint64_t size, const uint64_t offset = 0) const;

        // Resource tables
        ResourceTable& createResourceTable(const std::string& name, const ResourceTable::Layout& layout);
        ResourceTable& getResourceTable(const std::string& name) const;

        // Samplers
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

        struct ExternalImage
        {
            VkImageView vkImageView;
            VkExtent2D vkExtent2D;
        };

        struct Allocation
        {
            uint64_t rawOffset;
            uint64_t boundOffset;
            uint64_t size;
        };

        static constexpr std::array<VkDescriptorPoolSize, 4> kDescriptorPoolSizes = { {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 16 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 16 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16 },
            { VK_DESCRIPTOR_TYPE_SAMPLER, 16 },
        } };

        const VkImage& createImageResource(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage, const MemoryHeap::ResourceCategory category, const VkImageAspectFlags aspect);
        static VkImageView buildImageView(const Device& device, const ImageResource& imageResource, const ImageViewCreateInfo& viewInfo);
        static std::string buildImageViewKey(const std::string& name, const std::string& alias);
        void destroyViewsForResource(const std::string& name);

        static VkImage createVkImage(const Device& device, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage);
        static VkBuffer createVkBuffer(const Device& device, const uint64_t byteSize, const VkBufferUsageFlags usage);
        static Allocation allocateAndBindImage(const Device& device, MemoryHeap& memoryHeap, const MemoryHeap::ResourceCategory category, const VkImage image);
        static Allocation allocateAndBindBuffer(const Device& device, MemoryHeap& memoryHeap, const MemoryHeap::ResourceCategory category, const VkBuffer buffer);

        static void requireExists(const bool exists, const std::string& name, const char* kind);

    private:
        const Device& mDevice;

        MemoryHeap mMemoryHeap;
        DescriptorPool mDescriptorPool;

        std::unordered_map<std::string, ImageResource> mImages;
        std::unordered_map<std::string, VkImageView> mImageViews;
        std::unordered_map<std::string, ExternalImage> mExternalImages;
        std::unordered_map<std::string, BufferResource> mBuffers;
        mutable std::unordered_map<std::string, ResourceTable> mResourceTables;
        std::unordered_map<std::string, VkSampler> mSamplers;
    };
}
