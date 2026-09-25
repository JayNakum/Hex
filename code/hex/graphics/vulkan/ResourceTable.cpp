#include "ResourceTable.h"

#include "Device.h"

namespace graphics
{
    ResourceTable::ResourceTable(const Device& device, DescriptorPool& descriptorPool, const Layout& layout)
        : mDevice{ device }
        , mLayout{ layout }
        , mVkDescriptorSetLayout{ buildSetLayout(device, layout) }
        , mVkDescriptorSet{ descriptorPool.allocate(mVkDescriptorSetLayout) }
    {
    }

    ResourceTable::~ResourceTable()
    {
        if (mVkDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(mDevice.get(), mVkDescriptorSetLayout, nullptr);
            mVkDescriptorSetLayout = VK_NULL_HANDLE;
        }
    }

    ResourceTable::ResourceTable(ResourceTable&& other) noexcept
        : mDevice{ other.mDevice }
        , mLayout{ std::move(other.mLayout) }
        , mVkDescriptorSetLayout{ other.mVkDescriptorSetLayout }
        , mVkDescriptorSet{ other.mVkDescriptorSet }
    {
        other.mVkDescriptorSetLayout = VK_NULL_HANDLE;
        other.mVkDescriptorSet = VK_NULL_HANDLE;
    }

    void ResourceTable::setResource(const uint32_t binding, const VkDescriptorType vkDescriptorType, const VkBuffer& vkBuffer, const uint64_t offsetInBytes, const uint64_t rangeInBytes)
    {
        VkDescriptorBufferInfo vkDescriptorBufferInfo{};
        vkDescriptorBufferInfo.buffer = vkBuffer;
        vkDescriptorBufferInfo.offset = offsetInBytes;
        vkDescriptorBufferInfo.range = (rangeInBytes == 0) ? VK_WHOLE_SIZE : rangeInBytes;

        VkWriteDescriptorSet vkWriteDescriptorSet{};
        vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        vkWriteDescriptorSet.dstSet = mVkDescriptorSet;
        vkWriteDescriptorSet.dstBinding = binding;
        vkWriteDescriptorSet.dstArrayElement = 0;
        vkWriteDescriptorSet.descriptorCount = 1;
        vkWriteDescriptorSet.descriptorType = vkDescriptorType;
        vkWriteDescriptorSet.pBufferInfo = &vkDescriptorBufferInfo;

        vkUpdateDescriptorSets(mDevice.get(), 1, &vkWriteDescriptorSet, 0, nullptr);
    }

    void ResourceTable::setResource(const uint32_t binding, const VkDescriptorType vkDescriptorType, const VkImageView& vkImageView, const ResourceState& state, VkSampler vkSampler)
    {
        VkDescriptorImageInfo vkDescriptorImageInfo{};
        vkDescriptorImageInfo.sampler = vkSampler;
        vkDescriptorImageInfo.imageView = vkImageView;
        vkDescriptorImageInfo.imageLayout = state.vkImageLayout;

        VkWriteDescriptorSet vkWriteDescriptorSet{};
        vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        vkWriteDescriptorSet.dstSet = mVkDescriptorSet;
        vkWriteDescriptorSet.dstBinding = binding;
        vkWriteDescriptorSet.dstArrayElement = 0;
        vkWriteDescriptorSet.descriptorCount = 1;
        vkWriteDescriptorSet.descriptorType = vkDescriptorType;
        vkWriteDescriptorSet.pImageInfo = &vkDescriptorImageInfo;

        vkUpdateDescriptorSets(mDevice.get(), 1, &vkWriteDescriptorSet, 0, nullptr);
    }

    VkDescriptorSetLayout ResourceTable::buildSetLayout(const Device& device, const ResourceTable::Layout& layout)
    {
        std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
        vkDescriptorSetLayoutBindings.reserve(layout.ranges.size());

        for (size_t i = 0; i < layout.ranges.size(); ++i)
        {
            const ResourceTable::Range& range = layout.ranges[i];

            VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding{};
            vkDescriptorSetLayoutBinding.binding = static_cast<uint32_t>(i);
            vkDescriptorSetLayoutBinding.descriptorType = range.vkDescriptorType;
            vkDescriptorSetLayoutBinding.descriptorCount = range.count;
            vkDescriptorSetLayoutBinding.stageFlags = layout.vkShaderStageFlagsVisibility;
            vkDescriptorSetLayoutBinding.pImmutableSamplers = (range.vkImmutableSampler != VK_NULL_HANDLE) ? &range.vkImmutableSampler : nullptr;

            vkDescriptorSetLayoutBindings.push_back(vkDescriptorSetLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo{};
        vkDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        vkDescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(vkDescriptorSetLayoutBindings.size());
        vkDescriptorSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBindings.data();

        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        const VkResult vkResult = vkCreateDescriptorSetLayout(device.get(), &vkDescriptorSetLayoutCreateInfo, nullptr, &vkDescriptorSetLayout);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkDescriptorSetLayout. ", utility::translateErrorCode(vkResult).c_str());

        return vkDescriptorSetLayout;
    }
}
