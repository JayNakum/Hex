#include "MemoryHeap.h"

#include "Device.h"
#include "PhysicalDevice.h"

namespace graphics
{
    namespace
    {
        constexpr VkBufferUsageFlags BUFFER_PROBE_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
            | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        constexpr VkImageUsageFlags IMAGE_PROBE_USAGE = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
            | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    MemoryHeap::MemoryHeap(const Device& device, const PhysicalDevice& physicalDevice)
        : mDevice{ device }
        , mUploadMemoryTypeIndex{ findBufferMemoryTypeIndex(device, physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) }
        , mBufferMemoryTypeIndex{ findBufferMemoryTypeIndex(device, physicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) }
        , mImageMemoryTypeIndex{ findImageMemoryTypeIndex(device, physicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) }
        , mVkDeviceMemoryUpload{ allocateDeviceMemory(device, UPLOAD_BLOCK_SIZE, mUploadMemoryTypeIndex) }
        , mVkDeviceMemoryBuffer{ allocateDeviceMemory(device, BUFFER_BLOCK_SIZE, mBufferMemoryTypeIndex) }
        , mVkDeviceMemoryImage{ allocateDeviceMemory(device, IMAGE_BLOCK_SIZE, mImageMemoryTypeIndex) }
        , mUploadSections{ UPLOAD_BLOCK_SIZE }
        , mUploadData{ UPLOAD_DATA_SIZE, mUploadSections.allocate(UPLOAD_DATA_SIZE) }
        , mUploadPersistentData{ UPLOAD_PERSISTENT_DATA_SIZE, mUploadSections.allocate(UPLOAD_PERSISTENT_DATA_SIZE) }
        , mBufferSections{ BUFFER_BLOCK_SIZE }
        , mConstantsData{ CONSTANTS_DATA_SIZE, 256, mBufferSections.allocate(CONSTANTS_DATA_SIZE) }
        , mTransientData{ TRANSIENT_DATA_SIZE, mBufferSections.allocate(TRANSIENT_DATA_SIZE) }
        , mMeshData{ MESH_DATA_SIZE, 128, mBufferSections.allocate(MESH_DATA_SIZE) }
        , mPersistentData{ PERSISTENT_DATA_SIZE, mBufferSections.allocate(PERSISTENT_DATA_SIZE) }
        , mImageSections{ IMAGE_BLOCK_SIZE }
        , mRenderTargetData{ RENDER_TARGET_DATA_SIZE, mImageSections.allocate(RENDER_TARGET_DATA_SIZE) }
        , mTextureData{ TEXTURE_DATA_SIZE, mImageSections.allocate(TEXTURE_DATA_SIZE) }
    {
    }

    MemoryHeap::~MemoryHeap()
    {
        vkFreeMemory(mDevice.get(), mVkDeviceMemoryImage, nullptr);
        vkFreeMemory(mDevice.get(), mVkDeviceMemoryBuffer, nullptr);
        vkFreeMemory(mDevice.get(), mVkDeviceMemoryUpload, nullptr);
    }

    uint64_t MemoryHeap::allocate(const ResourceCategory category, const uint64_t sizeInBytes)
    {
        switch (category)
        {
        case ResourceCategory::UploadData:
            return mUploadData.allocate(sizeInBytes);
        case ResourceCategory::UploadPersistentData:
            return mUploadPersistentData.allocate(sizeInBytes);
        case ResourceCategory::ConstantsData:
            return mConstantsData.allocate(sizeInBytes);
        case ResourceCategory::TransientData:
            return mTransientData.allocate(sizeInBytes);
        case ResourceCategory::MeshData:
            return mMeshData.allocate(sizeInBytes);
        case ResourceCategory::PersistentData:
            return mPersistentData.allocate(sizeInBytes);
        case ResourceCategory::RenderTargetData:
            return mRenderTargetData.allocate(sizeInBytes);
        case ResourceCategory::TextureData:
            return mTextureData.allocate(sizeInBytes);
        default:
            HEX_ASSERT(false, "Invalid MemoryHeap::ResourceCategory ", enumCast(category));
            return 0;
        }
    }

    void MemoryHeap::deallocate(const ResourceCategory category, const uint64_t offset, const uint64_t sizeInBytes)
    {
        switch (category)
        {
        case ResourceCategory::UploadPersistentData:
            mUploadPersistentData.deallocate(offset, sizeInBytes);
            break;
        case ResourceCategory::ConstantsData:
            mConstantsData.deallocate(offset, sizeInBytes);
            break;
        case ResourceCategory::MeshData:
            mMeshData.deallocate(offset, sizeInBytes);
            break;
        case ResourceCategory::TextureData:
            mTextureData.deallocate(offset, sizeInBytes);
            break;
        case ResourceCategory::RenderTargetData:
            mRenderTargetData.deallocate(offset, sizeInBytes);
            break;
        case ResourceCategory::UploadData:
        case ResourceCategory::TransientData:
        case ResourceCategory::PersistentData:
            break;
        default:
            HEX_ASSERT(false, "Invalid MemoryHeap::ResourceCategory ", enumCast(category));
            break;
        }
    }

    void MemoryHeap::reset(const ResourceCategory category)
    {
        switch (category)
        {
        case ResourceCategory::UploadData:
            mUploadData.reset();
            break;
        case ResourceCategory::UploadPersistentData:
            mUploadPersistentData.reset();
            break;
        case ResourceCategory::ConstantsData:
            mConstantsData.reset();
            break;
        case ResourceCategory::TransientData:
            mTransientData.reset();
            break;
        case ResourceCategory::MeshData:
            mMeshData.reset();
            break;
        case ResourceCategory::PersistentData:
            mPersistentData.reset();
            break;
        case ResourceCategory::RenderTargetData:
            mRenderTargetData.reset();
            break;
        case ResourceCategory::TextureData:
            mTextureData.reset();
            break;
        default:
            HEX_ASSERT(false, "Invalid MemoryHeap::ResourceCategory ", enumCast(category));
            break;
        }
    }

    uint32_t MemoryHeap::getMemoryTypeIndex(const ResourceCategory category) const
    {
        switch (category)
        {
        case ResourceCategory::UploadData:
        case ResourceCategory::UploadPersistentData:
            return mUploadMemoryTypeIndex;
        case ResourceCategory::ConstantsData:
        case ResourceCategory::TransientData:
        case ResourceCategory::MeshData:
        case ResourceCategory::PersistentData:
            return mBufferMemoryTypeIndex;
        case ResourceCategory::RenderTargetData:
        case ResourceCategory::TextureData:
            return mImageMemoryTypeIndex;
        default:
            HEX_ASSERT(false, "Invalid MemoryHeap::ResourceCategory ", enumCast(category));
            return mBufferMemoryTypeIndex;
        }
    }

    void MemoryHeap::bindBufferMemory(const ResourceCategory category, VkBuffer vkBuffer, const uint64_t offset) const
    {
        const VkResult vkResult = vkBindBufferMemory(mDevice.get(), vkBuffer, getDeviceMemory(category), static_cast<VkDeviceSize>(offset));
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to bind VkBuffer memory. ", utility::translateErrorCode(vkResult).c_str());
    }

    void MemoryHeap::bindImageMemory(const ResourceCategory category, VkImage vkImage, const uint64_t offset) const
    {
        const VkResult vkResult = vkBindImageMemory(mDevice.get(), vkImage, getDeviceMemory(category), static_cast<VkDeviceSize>(offset));
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to bind VkImage memory. ", utility::translateErrorCode(vkResult).c_str());
    }

    void* MemoryHeap::map(const ResourceCategory category, const uint64_t size, const uint64_t offset) const
    {
        void* data = nullptr;
        const VkResult vkResult = vkMapMemory(mDevice.get(), getDeviceMemory(category), offset, size, 0, &data);
        HEX_ASSERT(vkResult == VK_SUCCESS && data, "Failed to map VkDeviceMemory. ", utility::translateErrorCode(vkResult).c_str());

        return data;
    }

    void MemoryHeap::flush(const ResourceCategory category, const uint64_t size, const uint64_t offset) const
    {
        VkMappedMemoryRange vkMappedMemoryRange{};
        vkMappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        vkMappedMemoryRange.memory = getDeviceMemory(category);
        vkMappedMemoryRange.offset = offset;
        vkMappedMemoryRange.size = size;

        const VkResult vkResult = vkFlushMappedMemoryRanges(mDevice.get(), 1, &vkMappedMemoryRange);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to flush mapped VkDeviceMemory. ", utility::translateErrorCode(vkResult).c_str());
    }

    void MemoryHeap::invalidate(const ResourceCategory category, const uint64_t size, const uint64_t offset) const
    {
        VkMappedMemoryRange vkMappedMemoryRange{};
        vkMappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        vkMappedMemoryRange.memory = getDeviceMemory(category);
        vkMappedMemoryRange.offset = offset;
        vkMappedMemoryRange.size = size;

        const VkResult vkResult = vkInvalidateMappedMemoryRanges(mDevice.get(), 1, &vkMappedMemoryRange);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to invalidate mapped VkDeviceMemory. ", utility::translateErrorCode(vkResult).c_str());
    }

    void MemoryHeap::unmap(const ResourceCategory category) const
    {
        vkUnmapMemory(mDevice.get(), getDeviceMemory(category));
    }

    uint32_t MemoryHeap::findBufferMemoryTypeIndex(const Device& device, const PhysicalDevice& physicalDevice, const VkMemoryPropertyFlags vkMemoryPropertyFlags)
    {
        VkBufferCreateInfo vkBufferCreateInfo{};
        vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkBufferCreateInfo.size = 256;
        vkBufferCreateInfo.usage = BUFFER_PROBE_USAGE;
        vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer vkBufferProbe = VK_NULL_HANDLE;
        const VkResult vkResultCreate = vkCreateBuffer(device.get(), &vkBufferCreateInfo, nullptr, &vkBufferProbe);
        HEX_ASSERT(vkResultCreate == VK_SUCCESS, "Failed to create probe VkBuffer. ", utility::translateErrorCode(vkResultCreate).c_str());

        VkMemoryRequirements vkMemoryRequirements{};
        vkGetBufferMemoryRequirements(device.get(), vkBufferProbe, &vkMemoryRequirements);

        vkDestroyBuffer(device.get(), vkBufferProbe, nullptr);

        return physicalDevice.findMemoryType(vkMemoryRequirements.memoryTypeBits, vkMemoryPropertyFlags);
    }

    uint32_t MemoryHeap::findImageMemoryTypeIndex(const Device& device, const PhysicalDevice& physicalDevice, const VkMemoryPropertyFlags vkMemoryPropertyFlags)
    {
        VkImageCreateInfo vkImageCreateInfo{};
        vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        vkImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        vkImageCreateInfo.extent = { 1, 1, 1 };
        vkImageCreateInfo.mipLevels = 1;
        vkImageCreateInfo.arrayLayers = 1;
        vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        vkImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        vkImageCreateInfo.usage = IMAGE_PROBE_USAGE;
        vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage vkImageProbe = VK_NULL_HANDLE;
        const VkResult vkResultCreate = vkCreateImage(device.get(), &vkImageCreateInfo, nullptr, &vkImageProbe);
        HEX_ASSERT(vkResultCreate == VK_SUCCESS, "Failed to create probe VkImage. ", utility::translateErrorCode(vkResultCreate).c_str());

        VkMemoryRequirements vkMemoryRequirements{};
        vkGetImageMemoryRequirements(device.get(), vkImageProbe, &vkMemoryRequirements);

        vkDestroyImage(device.get(), vkImageProbe, nullptr);

        return physicalDevice.findMemoryType(vkMemoryRequirements.memoryTypeBits, vkMemoryPropertyFlags);
    }

    VkDeviceMemory MemoryHeap::allocateDeviceMemory(const Device& device, const uint64_t size, const uint32_t memoryTypeIndex)
    {
        VkMemoryAllocateInfo vkMemoryAllocateInfo{};
        vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkMemoryAllocateInfo.allocationSize = static_cast<VkDeviceSize>(size);
        vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
        const VkResult vkResult = vkAllocateMemory(device.get(), &vkMemoryAllocateInfo, nullptr, &vkDeviceMemory);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to allocate VkDeviceMemory. ", utility::translateErrorCode(vkResult).c_str());

        return vkDeviceMemory;
    }

    VkDeviceMemory MemoryHeap::getDeviceMemory(const ResourceCategory category) const
    {
        switch (category)
        {
        case ResourceCategory::UploadData:
        case ResourceCategory::UploadPersistentData:
            return mVkDeviceMemoryUpload;
        case ResourceCategory::ConstantsData:
        case ResourceCategory::TransientData:
        case ResourceCategory::MeshData:
        case ResourceCategory::PersistentData:
            return mVkDeviceMemoryBuffer;
        case ResourceCategory::RenderTargetData:
        case ResourceCategory::TextureData:
            return mVkDeviceMemoryImage;
        default:
            HEX_ASSERT(false, "Invalid MemoryHeap::ResourceCategory ", enumCast(category));
            return mVkDeviceMemoryBuffer;
        }
    }
}
