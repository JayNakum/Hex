#pragma once

#include "vkCommon.h"
#include "resourceState.h"
#include "DescriptorPool.h"

namespace graphics
{
    class Device;

    class ResourceTable
    {
    public:
        struct Range
        {
            VkDescriptorType vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uint32_t count = 1;
        };

        struct Layout
        {
            VkShaderStageFlags vkShaderStageFlagsVisibility = VK_SHADER_STAGE_ALL;
            std::vector<Range> ranges;
        };

        ResourceTable(const Device& device, DescriptorPool& descriptorPool, const Layout& layout);
        ~ResourceTable();

        ResourceTable(const ResourceTable&) = delete;
        ResourceTable& operator=(const ResourceTable&) = delete;

        ResourceTable(ResourceTable&& other) noexcept;
        ResourceTable& operator=(ResourceTable&&) = delete;

        inline const Layout& getLayout() const { return mLayout; }
        inline VkDescriptorSetLayout getDescriptorSetLayout() const { return mVkDescriptorSetLayout; }
        inline VkDescriptorSet get() const { return mVkDescriptorSet; }

        void setResource(const uint32_t binding, const VkDescriptorType vkDescriptorType, const VkBuffer& vkBuffer, const uint64_t offsetInBytes = 0, const uint64_t rangeInBytes = 0);
        void setResource(const uint32_t binding, const VkDescriptorType vkDescriptorType, const VkImageView& vkImageView, const ResourceState& state, VkSampler vkSampler = VK_NULL_HANDLE);

    private:
        static VkDescriptorSetLayout buildSetLayout(const Device& device, const ResourceTable::Layout& layout);

    private:
        const Device& mDevice;
        Layout mLayout;
        VkDescriptorSetLayout mVkDescriptorSetLayout;
        VkDescriptorSet mVkDescriptorSet;
    };
}
