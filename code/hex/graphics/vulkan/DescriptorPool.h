#pragma once

#include "vkCommon.h"

namespace graphics
{
    class Device;

    class DescriptorPool
    {
    public:
        struct CreateInfo
        {
            const VkDescriptorPoolSize* vkDescriptorPoolSizes = nullptr;
            size_t vkDescriptorPoolSizeCount = 0;
        };

        DescriptorPool(const Device& device, const CreateInfo& createInfo);
        ~DescriptorPool();

        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        DescriptorPool(DescriptorPool&&) = delete;
        DescriptorPool& operator=(DescriptorPool&&) = delete;

        VkDescriptorSet allocate(const VkDescriptorSetLayout& layout);

        void reset();

    private:
        const Device& mDevice;

        VkDescriptorPool mVkDescriptorPool;
    };
}
