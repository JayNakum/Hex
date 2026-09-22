#include "ResourceManager.h"

#include "vulkan/Device.h"
#include "vulkan/PhysicalDevice.h"
#include "vulkan/CommandContext.h"

namespace graphics
{
    namespace
    {
        constexpr std::array<VkDescriptorPoolSize, 3> kDescriptorPoolSizes = { {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 16 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 16 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16 },
        } };

        struct Allocation
        {
            uint64_t rawOffset;
            uint64_t boundOffset;
            uint64_t size;
        };

        VkImage createVkImage(const Device& device, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage)
        {
            VkImageCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            createInfo.imageType = VK_IMAGE_TYPE_2D;
            createInfo.format = format;
            createInfo.extent = { width, height, 1 };
            createInfo.mipLevels = 1;
            createInfo.arrayLayers = 1;
            createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            createInfo.usage = usage;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkImage image = VK_NULL_HANDLE;
            const VkResult vkResult = vkCreateImage(device.get(), &createInfo, nullptr, &image);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkImage. ", utility::translateErrorCode(vkResult).c_str());

            return image;
        }

        VkBuffer createVkBuffer(const Device& device, const uint64_t byteSize, const VkBufferUsageFlags usage)
        {
            VkBufferCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createInfo.size = byteSize;
            createInfo.usage = usage;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VkBuffer buffer = VK_NULL_HANDLE;
            const VkResult vkResult = vkCreateBuffer(device.get(), &createInfo, nullptr, &buffer);
            HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkBuffer. ", utility::translateErrorCode(vkResult).c_str());

            return buffer;
        }

        Allocation allocateAndBindImage(const Device& device, MemoryHeap& memoryHeap, const MemoryHeap::ResourceCategory category, const VkImage image)
        {
            VkMemoryRequirements requirements{};
            vkGetImageMemoryRequirements(device.get(), image, &requirements);

            const uint32_t memoryTypeIndex = memoryHeap.getMemoryTypeIndex(category);
            HEX_ASSERT((requirements.memoryTypeBits & (1u << memoryTypeIndex)) != 0,
                "Selected memory type is incompatible with the image's requirements.");

            const uint64_t rawOffset = memoryHeap.allocate(category, requirements.size + requirements.alignment);
            const uint64_t alignedOffset = memory::alignUp<uint64_t>(rawOffset, static_cast<size_t>(requirements.alignment));

            memoryHeap.bindImageMemory(category, image, alignedOffset);
            return { rawOffset, alignedOffset, requirements.size + requirements.alignment };
        }

        Allocation allocateAndBindBuffer(const Device& device, MemoryHeap& memoryHeap, const MemoryHeap::ResourceCategory category, const VkBuffer buffer)
        {
            VkMemoryRequirements requirements{};
            vkGetBufferMemoryRequirements(device.get(), buffer, &requirements);

            const uint32_t memoryTypeIndex = memoryHeap.getMemoryTypeIndex(category);
            HEX_ASSERT((requirements.memoryTypeBits & (1u << memoryTypeIndex)) != 0,
                "Selected memory type is incompatible with the buffer's requirements.");

            const uint64_t rawOffset = memoryHeap.allocate(category, requirements.size + requirements.alignment);
            const uint64_t alignedOffset = memory::alignUp<uint64_t>(rawOffset, static_cast<size_t>(requirements.alignment));

            memoryHeap.bindBufferMemory(category, buffer, alignedOffset);
            return { rawOffset, alignedOffset, requirements.size + requirements.alignment };
        }
    }

    ResourceManager::ResourceManager(const Device& device, const PhysicalDevice& physicalDevice)
        : mDevice{ device }
        , mMemoryHeap{ device, physicalDevice }
        , mDescriptorPool{ device, DescriptorPool::CreateInfo{ kDescriptorPoolSizes.data(), kDescriptorPoolSizes.size() } }
        , mImages{}
        , mImageViews{}
        , mBuffers{}
        , mResourceTables{}
    {
    }

    ResourceManager::~ResourceManager()
    {
        for (std::unordered_map<std::string, VkImageView>::iterator it = mImageViews.begin(); it != mImageViews.end(); ++it)
        {
            vkDestroyImageView(mDevice.get(), it->second, nullptr);
        }

        for (std::unordered_map<std::string, BufferResource>::iterator it = mBuffers.begin(); it != mBuffers.end(); ++it)
        {
            vkDestroyBuffer(mDevice.get(), it->second.vkBuffer, nullptr);
        }

        for (std::unordered_map<std::string, ImageResource>::iterator it = mImages.begin(); it != mImages.end(); ++it)
        {
            vkDestroyImage(mDevice.get(), it->second.vkImage, nullptr);
        }
    }

    const VkImage& ResourceManager::createRenderTexture(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage, const MemoryHeap::ResourceCategory category)
    {
        return createImageResource(name, width, height, format, usage, category, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    const VkImage& ResourceManager::createDepthTexture(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format, const MemoryHeap::ResourceCategory category)
    {
        return createImageResource(name, width, height, format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, category, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    const VkImage& ResourceManager::recreateRenderTexture(const std::string& name, const uint32_t width, const uint32_t height)
    {
        const std::unordered_map<std::string, ImageResource>::iterator it = mImages.find(name);
        HEX_ASSERT(it != mImages.end(), "Image resource with ID ", name, " does not exist.");

        const MemoryHeap::ResourceCategory category = it->second.category;
        const VkFormat format = it->second.vkFormat;
        const VkImageUsageFlags usage = it->second.vkImageUsageFlags;
        const VkImageAspectFlags aspect = it->second.vkImageAspectFlags;
        const uint64_t allocationOffset = it->second.allocationOffset;
        const uint64_t allocationSize = it->second.allocationSize;

        destroyViewsForResource(name);
        vkDestroyImage(mDevice.get(), it->second.vkImage, nullptr);
        mMemoryHeap.deallocate(category, allocationOffset, allocationSize);
        mImages.erase(it);

        return createImageResource(name, width, height, format, usage, category, aspect);
    }

    VkBuffer& ResourceManager::createBuffer(const std::string& name, const BufferCreateInfo& createInfo, const MemoryHeap::ResourceCategory category)
    {
        const VkBuffer vkBuffer = createVkBuffer(mDevice, createInfo.byteSize, createInfo.usage);
        const Allocation allocation = allocateAndBindBuffer(mDevice, mMemoryHeap, category, vkBuffer);

        const std::pair<std::unordered_map<std::string, BufferResource>::iterator, bool> insertResult = mBuffers.try_emplace(name, BufferResource{ vkBuffer, category, allocation.rawOffset, allocation.size });
        HEX_ASSERT(insertResult.second, "Buffer resource with ID ", name, " already exists.");

        return insertResult.first->second.vkBuffer;
    }

    VkImage& ResourceManager::getImage(const std::string& name)
    {
        const std::unordered_map<std::string, ImageResource>::iterator it = mImages.find(name);
        HEX_ASSERT(it != mImages.end(), "Image resource with ID ", name, " does not exist.");
        return it->second.vkImage;
    }

    const VkImage& ResourceManager::getImage(const std::string& name) const
    {
        const std::unordered_map<std::string, ImageResource>::const_iterator it = mImages.find(name);
        HEX_ASSERT(it != mImages.end(), "Image resource with ID ", name, " does not exist.");
        return it->second.vkImage;
    }

    VkExtent2D ResourceManager::getExtent(const std::string& name) const
    {
        const std::unordered_map<std::string, ImageResource>::const_iterator it = mImages.find(name);
        HEX_ASSERT(it != mImages.end(), "Image resource with ID ", name, " does not exist.");
        return it->second.vkExtent2D;
    }

    VkBuffer& ResourceManager::getBuffer(const std::string& name)
    {
        const std::unordered_map<std::string, BufferResource>::iterator it = mBuffers.find(name);
        HEX_ASSERT(it != mBuffers.end(), "Buffer resource with ID ", name, " does not exist.");
        return it->second.vkBuffer;
    }

    const VkBuffer& ResourceManager::getBuffer(const std::string& name) const
    {
        const std::unordered_map<std::string, BufferResource>::const_iterator it = mBuffers.find(name);
        HEX_ASSERT(it != mBuffers.end(), "Buffer resource with ID ", name, " does not exist.");
        return it->second.vkBuffer;
    }

    void ResourceManager::writeBuffer(const std::string& name, const void* data, const uint64_t size, const uint64_t offset)
    {
        const std::unordered_map<std::string, BufferResource>::iterator it = mBuffers.find(name);
        HEX_ASSERT(it != mBuffers.end(), "Buffer resource with ID ", name, " does not exist.");

        const BufferResource& bufferResource = it->second;
        void* mapped = mMemoryHeap.map(bufferResource.category, size, bufferResource.allocationOffset + offset);
        std::memcpy(mapped, data, size);
        mMemoryHeap.unmap(bufferResource.category);
    }

    void ResourceManager::transitionImage(CommandContext& context, const std::string& name, const ResourceState& newState)
    {
        const std::unordered_map<std::string, ImageResource>::iterator it = mImages.find(name);
        HEX_ASSERT(it != mImages.end(), "Image resource with ID ", name, " does not exist.");

        ImageResource& imageResource = it->second;
        context.transitionBarrier(imageResource.vkImage, imageResource.currentState, newState, imageResource.vkImageAspectFlags);
        imageResource.currentState = newState;
    }

    VkImageView ResourceManager::createImageView(const std::string& name, const ImageViewCreateInfo& viewInfo, const std::string& alias)
    {
        const std::unordered_map<std::string, ImageResource>::iterator it = mImages.find(name);
        HEX_ASSERT(it != mImages.end(), "Image resource with ID ", name, " does not exist.");

        const std::string key = buildImageViewKey(name, alias);
        const std::pair<std::unordered_map<std::string, VkImageView>::iterator, bool> insertResult = mImageViews.try_emplace(key, VK_NULL_HANDLE);
        HEX_ASSERT(insertResult.second, "Image view with alias ", alias, " already exists for image resource ", name, ".");

        insertResult.first->second = buildImageView(mDevice, it->second, viewInfo);
        return insertResult.first->second;
    }

    VkImageView ResourceManager::getImageView(const std::string& name, const std::string& alias)
    {
        const std::string key = buildImageViewKey(name, alias);
        const std::unordered_map<std::string, VkImageView>::iterator viewIt = mImageViews.find(key);
        if (viewIt != mImageViews.end())
        {
            return viewIt->second;
        }

        const std::unordered_map<std::string, ImageResource>::iterator it = mImages.find(name);
        HEX_ASSERT(it != mImages.end(), "Image resource with ID ", name, " does not exist.");

        const VkImageView vkImageView = buildImageView(mDevice, it->second, ImageViewCreateInfo{});
        mImageViews.emplace(key, vkImageView);
        return vkImageView;
    }

    void ResourceManager::destroyImageView(const std::string& name, const std::string& alias)
    {
        const std::string key = buildImageViewKey(name, alias);
        const std::unordered_map<std::string, VkImageView>::iterator it = mImageViews.find(key);
        HEX_ASSERT(it != mImageViews.end(), "Image view with alias ", alias, " does not exist for image resource ", name, ".");

        vkDestroyImageView(mDevice.get(), it->second, nullptr);
        mImageViews.erase(it);
    }

    ResourceTable& ResourceManager::createResourceTable(const std::string& name, const ResourceTable::Layout& layout)
    {
        const std::pair<std::unordered_map<std::string, ResourceTable>::iterator, bool> insertResult = mResourceTables.try_emplace(name, mDevice, mDescriptorPool, layout);
        HEX_ASSERT(insertResult.second, "ResourceTable resource with ID ", name, " already exists.");

        return insertResult.first->second;
    }

    ResourceTable& ResourceManager::getResourceTable(const std::string& name)
    {
        const std::unordered_map<std::string, ResourceTable>::iterator it = mResourceTables.find(name);
        HEX_ASSERT(it != mResourceTables.end(), "ResourceTable resource with ID ", name, " does not exist.");
        return it->second;
    }

    const VkImage& ResourceManager::createImageResource(const std::string& name, const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usage, const MemoryHeap::ResourceCategory category, const VkImageAspectFlags aspect)
    {
        const VkImage vkImage = createVkImage(mDevice, width, height, format, usage);
        const Allocation allocation = allocateAndBindImage(mDevice, mMemoryHeap, category, vkImage);
        const VkExtent2D vkExtent2D{ width, height };

        const std::pair<std::unordered_map<std::string, ImageResource>::iterator, bool> insertResult = mImages.try_emplace(name, ImageResource{ vkImage, format, vkExtent2D, usage, aspect, ResourceStates::Undefined, category, allocation.rawOffset, allocation.size });
        HEX_ASSERT(insertResult.second, "Image resource with ID ", name, " already exists.");

        return insertResult.first->second.vkImage;
    }

    VkImageView ResourceManager::buildImageView(const Device& device, const ImageResource& imageResource, const ImageViewCreateInfo& viewInfo)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = imageResource.vkImage;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = (viewInfo.vkFormat != VK_FORMAT_UNDEFINED) ? viewInfo.vkFormat : imageResource.vkFormat;
        createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        createInfo.subresourceRange = {
            (viewInfo.vkImageAspectFlags != 0) ? viewInfo.vkImageAspectFlags : imageResource.vkImageAspectFlags,
            viewInfo.baseMipLevel, viewInfo.levelCount,
            viewInfo.baseArrayLayer, viewInfo.layerCount
        };

        VkImageView vkImageView = VK_NULL_HANDLE;
        const VkResult vkResult = vkCreateImageView(device.get(), &createInfo, nullptr, &vkImageView);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkImageView. ", utility::translateErrorCode(vkResult).c_str());

        return vkImageView;
    }

    std::string ResourceManager::buildImageViewKey(const std::string& name, const std::string& alias)
    {
        return name + "#" + alias;
    }

    void ResourceManager::destroyViewsForResource(const std::string& name)
    {
        const std::string prefix = name + "#";
        for (std::unordered_map<std::string, VkImageView>::iterator it = mImageViews.begin(); it != mImageViews.end();)
        {
            if (it->first.compare(0, prefix.size(), prefix) == 0)
            {
                vkDestroyImageView(mDevice.get(), it->second, nullptr);
                it = mImageViews.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}
