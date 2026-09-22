#include "DescriptorPool.h"

#include "Device.h"

namespace graphics
{
    DescriptorPool::DescriptorPool(const Device& device, const CreateInfo& createInfo)
        : mDevice{ device }
        , mVkDescriptorPool{ VK_NULL_HANDLE }
    {
        VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo{};
        vkDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        vkDescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        uint32_t maxSets = 0;
        for (size_t i = 0; i < createInfo.vkDescriptorPoolSizeCount; ++i)
        {
            maxSets += createInfo.vkDescriptorPoolSizes[i].descriptorCount;
        }
        vkDescriptorPoolCreateInfo.maxSets = maxSets;
        vkDescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(createInfo.vkDescriptorPoolSizeCount);
        vkDescriptorPoolCreateInfo.pPoolSizes = createInfo.vkDescriptorPoolSizes;

        const VkResult vkResult = vkCreateDescriptorPool(device.get(), &vkDescriptorPoolCreateInfo, nullptr, &mVkDescriptorPool);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkDescriptorPool. ", utility::translateErrorCode(vkResult).c_str());
    }

    DescriptorPool::~DescriptorPool()
    {
        vkDestroyDescriptorPool(mDevice.get(), mVkDescriptorPool, nullptr);
    }

    VkDescriptorSet DescriptorPool::allocate(const VkDescriptorSetLayout& layout)
    {
        VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo{};
        vkDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        vkDescriptorSetAllocateInfo.descriptorPool = mVkDescriptorPool;
        vkDescriptorSetAllocateInfo.descriptorSetCount = 1;
        vkDescriptorSetAllocateInfo.pSetLayouts = &layout;

        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        const VkResult vkResult = vkAllocateDescriptorSets(mDevice.get(), &vkDescriptorSetAllocateInfo, &vkDescriptorSet);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to allocate VkDescriptorSet. ", utility::translateErrorCode(vkResult).c_str());

        return vkDescriptorSet;
    }

    void DescriptorPool::reset()
    {
        const VkResult vkResult = vkResetDescriptorPool(mDevice.get(), mVkDescriptorPool, 0);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to reset VkDescriptorPool. ", utility::translateErrorCode(vkResult).c_str());
    }
}
