#pragma once

#include "vkCommon.h"

#include "core/memory/LinearAllocator.h"
#include "core/memory/RingAllocator.h"
#include "core/memory/BlockAllocator.h"
#include "core/memory/SuballocationAllocator.h"

namespace graphics
{
    class Device;
    class PhysicalDevice;

    class MemoryHeap
    {
    public:
        enum class ResourceCategory
        {
            UploadData,
            UploadPersistentData,

            ConstantsData,
            TransientData,
            MeshData,
            PersistentData,

            RenderTargetData,
            TextureData,
        };

        MemoryHeap(const Device& device, const PhysicalDevice& physicalDevice);
        ~MemoryHeap();

        MemoryHeap(const MemoryHeap&) = delete;
        MemoryHeap& operator=(const MemoryHeap&) = delete;

        MemoryHeap(MemoryHeap&&) = delete;
        MemoryHeap& operator=(MemoryHeap&&) = delete;

        uint64_t allocate(const ResourceCategory category, const uint64_t sizeInBytes);
        void deallocate(const ResourceCategory category, const uint64_t offset, const uint64_t sizeInBytes);
        void reset(const ResourceCategory category);

        uint32_t getMemoryTypeIndex(const ResourceCategory category) const;

        void bindBufferMemory(const ResourceCategory category, VkBuffer vkBuffer, const uint64_t offset) const;
        void bindImageMemory(const ResourceCategory category, VkImage vkImage, const uint64_t offset) const;

        void* map(const ResourceCategory category, const uint64_t size, const uint64_t offset = 0) const;
        void flush(const ResourceCategory category, const uint64_t size, const uint64_t offset = 0) const;
        void invalidate(const ResourceCategory category, const uint64_t size, const uint64_t offset = 0) const;
        void unmap(const ResourceCategory category) const;

    private:
        static uint32_t findBufferMemoryTypeIndex(const Device& device, const PhysicalDevice& physicalDevice, const VkMemoryPropertyFlags vkMemoryPropertyFlags);
        static uint32_t findImageMemoryTypeIndex(const Device& device, const PhysicalDevice& physicalDevice, const VkMemoryPropertyFlags vkMemoryPropertyFlags);
        static VkDeviceMemory allocateDeviceMemory(const Device& device, const uint64_t size, const uint32_t memoryTypeIndex);

        VkDeviceMemory getDeviceMemory(const ResourceCategory category) const;

    private:
        static constexpr uint64_t UPLOAD_DATA_SIZE = memory::MB(32);
        static constexpr uint64_t UPLOAD_PERSISTENT_DATA_SIZE = memory::MB(16);
        static constexpr uint64_t UPLOAD_BLOCK_SIZE = UPLOAD_DATA_SIZE + UPLOAD_PERSISTENT_DATA_SIZE;

        static constexpr uint64_t CONSTANTS_DATA_SIZE = memory::MB(16);
        static constexpr uint64_t TRANSIENT_DATA_SIZE = memory::MB(32);
        static constexpr uint64_t MESH_DATA_SIZE = memory::MB(64);
        static constexpr uint64_t PERSISTENT_DATA_SIZE = memory::MB(32);
        static constexpr uint64_t BUFFER_BLOCK_SIZE = CONSTANTS_DATA_SIZE + TRANSIENT_DATA_SIZE + MESH_DATA_SIZE + PERSISTENT_DATA_SIZE;

        static constexpr uint64_t RENDER_TARGET_DATA_SIZE = memory::MB(64);
        static constexpr uint64_t TEXTURE_DATA_SIZE = memory::MB(128);
        static constexpr uint64_t IMAGE_BLOCK_SIZE = RENDER_TARGET_DATA_SIZE + TEXTURE_DATA_SIZE;

    private:
        const Device& mDevice;

        uint32_t mUploadMemoryTypeIndex;
        uint32_t mBufferMemoryTypeIndex;
        uint32_t mImageMemoryTypeIndex;

        VkDeviceMemory mVkDeviceMemoryUpload;
        VkDeviceMemory mVkDeviceMemoryBuffer;
        VkDeviceMemory mVkDeviceMemoryImage;

        memory::LinearAllocator mUploadSections;
        memory::RingAllocator mUploadData;
        memory::SuballocationAllocator mUploadPersistentData;

        memory::LinearAllocator mBufferSections;
        memory::BlockAllocator mConstantsData;
        memory::RingAllocator mTransientData;
        memory::BlockAllocator mMeshData;
        memory::LinearAllocator mPersistentData;

        memory::LinearAllocator mImageSections;
        memory::SuballocationAllocator mRenderTargetData;
        memory::SuballocationAllocator mTextureData;
    };
}
