#include "ShaderInput.h"

#include "Device.h"

#include "CommandContext.h"
#include "ResourceTable.h"

namespace graphics
{
    ShaderInput::ShaderInput(const CreateInfo& createInfo)
        : mVkDescriptorSetLayouts{ createInfo.vkDescriptorSetLayouts }
        , mVkPushConstantRanges{ createInfo.vkPushConstantRanges }
        , mConstantsData{}
        , mResourceTables{ mVkDescriptorSetLayouts.size(), nullptr }
    {
        mConstantsData.reserve(mVkPushConstantRanges.size());
        for (const VkPushConstantRange& vkPushConstantRange : mVkPushConstantRanges)
        {
            mConstantsData.emplace_back(vkPushConstantRange.size, uint8_t{ 0 });
        }
    }

    VkPipelineLayout ShaderInput::getPipelineLayout(const Device& device) const
    {
        const std::string signature = buildSignature();

        std::unordered_map<std::string, CachedPipelineLayout>& cache = pipelineLayoutCache();
        const std::unordered_map<std::string, CachedPipelineLayout>::iterator it = cache.find(signature);
        if (it != cache.end())
        {
            return it->second.vkPipelineLayout;
        }

        const VkPipelineLayout vkPipelineLayout = ShaderInput::buildPipelineLayout(device, mVkDescriptorSetLayouts, mVkPushConstantRanges);
        cache.emplace(signature, CachedPipelineLayout{ device.get(), vkPipelineLayout });

        return vkPipelineLayout;
    }

    std::unordered_map<std::string, ShaderInput::CachedPipelineLayout>& ShaderInput::pipelineLayoutCache()
    {
        static std::unordered_map<std::string, ShaderInput::CachedPipelineLayout> cache;
        return cache;
    }

    void ShaderInput::clearPipelineLayoutCache()
    {
        std::unordered_map<std::string, CachedPipelineLayout>& cache = pipelineLayoutCache();
        for (std::unordered_map<std::string, CachedPipelineLayout>::iterator it = cache.begin(); it != cache.end(); ++it)
        {
            vkDestroyPipelineLayout(it->second.vkDevice, it->second.vkPipelineLayout, nullptr);
        }
        cache.clear();
    }

    void ShaderInput::setConstants(const uint32_t rangeIndex, const void* data, const uint32_t size)
    {
        HEX_ASSERT(rangeIndex < mConstantsData.size(), "ShaderInput push-constant range index out of bounds.");
        HEX_ASSERT(size == mVkPushConstantRanges[rangeIndex].size, "ShaderInput::setConstants size does not match the declared push-constant range size.");

        std::memcpy(mConstantsData[rangeIndex].data(), data, size);
    }

    void ShaderInput::setResourceTable(const uint32_t setIndex, const ResourceTable& table)
    {
        HEX_ASSERT(setIndex < mResourceTables.size(), "ShaderInput resource table set index out of bounds.");
        mResourceTables[setIndex] = &table;
    }

    void ShaderInput::finalize(CommandContext& context) const
    {
        const VkPipelineLayout vkPipelineLayout = getPipelineLayout(context.getDevice());

        for (size_t i = 0; i < mVkPushConstantRanges.size(); ++i)
        {
            const VkPushConstantRange& vkPushConstantRange = mVkPushConstantRanges[i];
            context.pushConstants(vkPipelineLayout, vkPushConstantRange.stageFlags, vkPushConstantRange.offset, vkPushConstantRange.size, mConstantsData[i].data());
        }

        std::vector<VkDescriptorSet> vkDescriptorSets;
        vkDescriptorSets.reserve(mResourceTables.size());
        for (const ResourceTable* table : mResourceTables)
        {
            HEX_ASSERT(table != nullptr, "Finalize called with an unset resource table.");
            vkDescriptorSets.push_back(table->get());
        }

        if (!vkDescriptorSets.empty())
        {
            context.bindDescriptorSets(vkPipelineLayout, 0, vkDescriptorSets);
        }
    }

    std::string ShaderInput::buildSignature() const
    {
        std::ostringstream stream;

        for (const VkDescriptorSetLayout vkDescriptorSetLayout : mVkDescriptorSetLayouts)
        {
            stream << reinterpret_cast<uintptr_t>(vkDescriptorSetLayout) << ';';
        }
        stream << '|';
        for (const VkPushConstantRange& vkPushConstantRange : mVkPushConstantRanges)
        {
            stream << static_cast<uint32_t>(vkPushConstantRange.stageFlags) << ',' << vkPushConstantRange.offset << ',' << vkPushConstantRange.size << ';';
        }

        return stream.str();
    }

    VkPipelineLayout ShaderInput::buildPipelineLayout(const Device& device, const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkPushConstantRange>& vkPushConstantRanges)
    {
        VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo{};
        vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        vkPipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(vkDescriptorSetLayouts.size());
        vkPipelineLayoutCreateInfo.pSetLayouts = vkDescriptorSetLayouts.data();
        vkPipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(vkPushConstantRanges.size());
        vkPipelineLayoutCreateInfo.pPushConstantRanges = vkPushConstantRanges.data();

        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        const VkResult vkResult = vkCreatePipelineLayout(device.get(), &vkPipelineLayoutCreateInfo, nullptr, &vkPipelineLayout);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkPipelineLayout. ", utility::translateErrorCode(vkResult).c_str());

        return vkPipelineLayout;
    }
}
