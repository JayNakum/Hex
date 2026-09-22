#pragma once

#include "vkCommon.h"

namespace graphics
{
    class Device;

    class Pipeline
    {
    public:
        struct ShaderStageInfo
        {
            VkShaderStageFlagBits vkShaderStageFlagBits = VK_SHADER_STAGE_VERTEX_BIT;
            VkShaderModule vkShaderModule = VK_NULL_HANDLE;
            std::string entryPoint = "main";
        };

        struct VertexInputBinding
        {
            uint32_t binding = 0;
            uint32_t stride = 0;
            VkVertexInputRate vkVertexInputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        };

        struct VertexInputAttribute
        {
            uint32_t location = 0;
            uint32_t binding = 0;
            VkFormat vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            uint32_t offset = 0;
        };

        struct ColorBlendAttachment
        {
            bool blendEnable = false;
            VkBlendFactor vkBlendFactorSrcColor = VK_BLEND_FACTOR_ONE;
            VkBlendFactor vkBlendFactorDstColor = VK_BLEND_FACTOR_ZERO;
            VkBlendOp vkBlendOpColor = VK_BLEND_OP_ADD;
            VkBlendFactor vkBlendFactorSrcAlpha = VK_BLEND_FACTOR_ONE;
            VkBlendFactor vkBlendFactorDstAlpha = VK_BLEND_FACTOR_ZERO;
            VkBlendOp vkBlendOpAlpha = VK_BLEND_OP_ADD;
            VkColorComponentFlags vkColorComponentFlagsWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        };

        struct GraphicsCreateInfo
        {
            std::vector<ShaderStageInfo> shaderStages;

            std::vector<VertexInputBinding> vertexBindings;
            std::vector<VertexInputAttribute> vertexAttributes;

            VkPrimitiveTopology vkPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            VkPolygonMode vkPolygonMode = VK_POLYGON_MODE_FILL;
            VkCullModeFlags vkCullModeFlags = VK_CULL_MODE_BACK_BIT;
            VkFrontFace vkFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

            std::vector<VkFormat> vkFormatsColorAttachment;
            std::vector<ColorBlendAttachment> colorBlendAttachments;
            VkFormat vkFormatDepthAttachment = VK_FORMAT_UNDEFINED;

            bool depthTestEnable = false;
            bool depthWriteEnable = false;
            VkCompareOp vkCompareOpDepth = VK_COMPARE_OP_LESS;

            VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        };

        struct ComputeCreateInfo
        {
            VkShaderModule vkShaderModule = VK_NULL_HANDLE;
            std::string entryPoint = "computeMain";

            VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        };

        Pipeline(const Device& device, const GraphicsCreateInfo& createInfo);
        Pipeline(const Device& device, const ComputeCreateInfo& createInfo);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        Pipeline(Pipeline&& other) noexcept;
        Pipeline& operator=(Pipeline&&) = delete;

        inline VkPipeline get() const { return mVkPipeline; }
        inline VkPipelineBindPoint getBindPoint() const { return mVkPipelineBindPoint; }

    private:
        const Device& mDevice;
        VkPipeline mVkPipeline;
        VkPipelineBindPoint mVkPipelineBindPoint;
    };
}
