#pragma once

#include "vkCommon.h"

namespace graphics
{
    class Device;
    class CommandContext;
    class ResourceTable;

    class ShaderInput
    {
    public:
        struct CreateInfo
        {
            std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;
            std::vector<VkPushConstantRange> vkPushConstantRanges;
        };

        ShaderInput(const CreateInfo& createInfo);

        ShaderInput(const ShaderInput&) = delete;
        ShaderInput& operator=(const ShaderInput&) = delete;

        ShaderInput(ShaderInput&&) noexcept = default;
        ShaderInput& operator=(ShaderInput&&) = delete;

        VkPipelineLayout getPipelineLayout(const Device& device) const;

        struct CachedPipelineLayout
        {
            VkDevice vkDevice = VK_NULL_HANDLE;
            VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        };
        static std::unordered_map<std::string, CachedPipelineLayout>& pipelineLayoutCache();
        static void clearPipelineLayoutCache();

        void setConstants(const uint32_t rangeIndex, const void* data, const uint32_t size);
        void setResourceTable(const uint32_t setIndex, const ResourceTable& table);

        void finalize(CommandContext& context) const;

    private:
        std::string buildSignature() const;
        static VkPipelineLayout buildPipelineLayout(const Device& device, const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkPushConstantRange>& vkPushConstantRanges);

    private:
        std::vector<VkDescriptorSetLayout> mVkDescriptorSetLayouts;
        std::vector<VkPushConstantRange> mVkPushConstantRanges;

        std::vector<std::vector<uint8_t>> mConstantsData;
        std::vector<const ResourceTable*> mResourceTables;
    };
}
