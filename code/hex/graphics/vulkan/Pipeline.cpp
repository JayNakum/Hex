#include "Pipeline.h"

#include "Device.h"

namespace graphics
{
    Pipeline::Pipeline(const Device& device, const GraphicsCreateInfo& createInfo)
        : mDevice{ device }
        , mVkPipeline{ VK_NULL_HANDLE }
        , mVkPipelineBindPoint{ VK_PIPELINE_BIND_POINT_GRAPHICS }
    {
        std::vector<VkPipelineShaderStageCreateInfo> vkPipelineShaderStageCreateInfos;
        vkPipelineShaderStageCreateInfos.reserve(createInfo.shaderStages.size());

        for (const ShaderStageInfo& stage : createInfo.shaderStages)
        {
            HEX_ASSERT(stage.vkShaderModule != VK_NULL_HANDLE, "Invalid null VkShaderModule in Pipeline::GraphicsCreateInfo shader stage");

            VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo{};
            vkPipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vkPipelineShaderStageCreateInfo.stage = stage.vkShaderStageFlagBits;
            vkPipelineShaderStageCreateInfo.module = stage.vkShaderModule;
            vkPipelineShaderStageCreateInfo.pName = stage.entryPoint.c_str();

            vkPipelineShaderStageCreateInfos.push_back(vkPipelineShaderStageCreateInfo);
        }

        std::vector<VkVertexInputBindingDescription> vkVertexInputBindingDescriptions;
        vkVertexInputBindingDescriptions.reserve(createInfo.vertexBindings.size());
        for (const VertexInputBinding& binding : createInfo.vertexBindings)
        {
            VkVertexInputBindingDescription vkVertexInputBindingDescription{};
            vkVertexInputBindingDescription.binding = binding.binding;
            vkVertexInputBindingDescription.stride = binding.stride;
            vkVertexInputBindingDescription.inputRate = binding.vkVertexInputRate;

            vkVertexInputBindingDescriptions.push_back(vkVertexInputBindingDescription);
        }

        std::vector<VkVertexInputAttributeDescription> vkVertexInputAttributeDescriptions;
        vkVertexInputAttributeDescriptions.reserve(createInfo.vertexAttributes.size());
        for (const VertexInputAttribute& attribute : createInfo.vertexAttributes)
        {
            VkVertexInputAttributeDescription vkVertexInputAttributeDescription{};
            vkVertexInputAttributeDescription.location = attribute.location;
            vkVertexInputAttributeDescription.binding = attribute.binding;
            vkVertexInputAttributeDescription.format = attribute.vkFormat;
            vkVertexInputAttributeDescription.offset = attribute.offset;

            vkVertexInputAttributeDescriptions.push_back(vkVertexInputAttributeDescription);
        }

        VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo{};
        vkPipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vkPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vkVertexInputBindingDescriptions.size());
        vkPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vkVertexInputBindingDescriptions.data();
        vkPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vkVertexInputAttributeDescriptions.size());
        vkPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vkVertexInputAttributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo{};
        vkPipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        vkPipelineInputAssemblyStateCreateInfo.topology = createInfo.vkPrimitiveTopology;
        vkPipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo{};
        vkPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vkPipelineViewportStateCreateInfo.viewportCount = 1;
        vkPipelineViewportStateCreateInfo.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo{};
        vkPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        vkPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        vkPipelineRasterizationStateCreateInfo.polygonMode = createInfo.vkPolygonMode;
        vkPipelineRasterizationStateCreateInfo.cullMode = createInfo.vkCullModeFlags;
        vkPipelineRasterizationStateCreateInfo.frontFace = createInfo.vkFrontFace;
        vkPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo{};
        vkPipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        vkPipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        vkPipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo{};
        vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        vkPipelineDepthStencilStateCreateInfo.depthTestEnable = createInfo.depthTestEnable ? VK_TRUE : VK_FALSE;
        vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = createInfo.depthWriteEnable ? VK_TRUE : VK_FALSE;
        vkPipelineDepthStencilStateCreateInfo.depthCompareOp = createInfo.vkCompareOpDepth;
        vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

        std::vector<VkPipelineColorBlendAttachmentState> vkPipelineColorBlendAttachmentStates;
        vkPipelineColorBlendAttachmentStates.reserve(createInfo.colorBlendAttachments.size());
        for (const ColorBlendAttachment& attachment : createInfo.colorBlendAttachments)
        {
            VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState{};
            vkPipelineColorBlendAttachmentState.blendEnable = attachment.blendEnable ? VK_TRUE : VK_FALSE;
            vkPipelineColorBlendAttachmentState.srcColorBlendFactor = attachment.vkBlendFactorSrcColor;
            vkPipelineColorBlendAttachmentState.dstColorBlendFactor = attachment.vkBlendFactorDstColor;
            vkPipelineColorBlendAttachmentState.colorBlendOp = attachment.vkBlendOpColor;
            vkPipelineColorBlendAttachmentState.srcAlphaBlendFactor = attachment.vkBlendFactorSrcAlpha;
            vkPipelineColorBlendAttachmentState.dstAlphaBlendFactor = attachment.vkBlendFactorDstAlpha;
            vkPipelineColorBlendAttachmentState.alphaBlendOp = attachment.vkBlendOpAlpha;
            vkPipelineColorBlendAttachmentState.colorWriteMask = attachment.vkColorComponentFlagsWriteMask;

            vkPipelineColorBlendAttachmentStates.push_back(vkPipelineColorBlendAttachmentState);
        }

        VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo{};
        vkPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        vkPipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        vkPipelineColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(vkPipelineColorBlendAttachmentStates.size());
        vkPipelineColorBlendStateCreateInfo.pAttachments = vkPipelineColorBlendAttachmentStates.data();

        const VkDynamicState vkDynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo{};
        vkPipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        vkPipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(std::size(vkDynamicStates));
        vkPipelineDynamicStateCreateInfo.pDynamicStates = vkDynamicStates;

        VkPipelineRenderingCreateInfo vkPipelineRenderingCreateInfo{};
        vkPipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        vkPipelineRenderingCreateInfo.colorAttachmentCount = static_cast<uint32_t>(createInfo.vkFormatsColorAttachment.size());
        vkPipelineRenderingCreateInfo.pColorAttachmentFormats = createInfo.vkFormatsColorAttachment.data();
        vkPipelineRenderingCreateInfo.depthAttachmentFormat = createInfo.vkFormatDepthAttachment;
        vkPipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo{};
        vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        vkGraphicsPipelineCreateInfo.pNext = &vkPipelineRenderingCreateInfo;
        vkGraphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(vkPipelineShaderStageCreateInfos.size());
        vkGraphicsPipelineCreateInfo.pStages = vkPipelineShaderStageCreateInfos.data();
        vkGraphicsPipelineCreateInfo.pVertexInputState = &vkPipelineVertexInputStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pViewportState = &vkPipelineViewportStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pMultisampleState = &vkPipelineMultisampleStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pColorBlendState = &vkPipelineColorBlendStateCreateInfo;
        vkGraphicsPipelineCreateInfo.pDynamicState = &vkPipelineDynamicStateCreateInfo;
        vkGraphicsPipelineCreateInfo.layout = createInfo.vkPipelineLayout;
        vkGraphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;
        vkGraphicsPipelineCreateInfo.subpass = 0;

        const VkResult vkResult = vkCreateGraphicsPipelines(mDevice.get(), VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, nullptr, &mVkPipeline);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create graphics VkPipeline. ", utility::translateErrorCode(vkResult).c_str());
    }

    Pipeline::Pipeline(const Device& device, const ComputeCreateInfo& createInfo)
        : mDevice{ device }
        , mVkPipeline{ VK_NULL_HANDLE }
        , mVkPipelineBindPoint{ VK_PIPELINE_BIND_POINT_COMPUTE }
    {
        HEX_ASSERT(createInfo.vkShaderModule != VK_NULL_HANDLE, "Invalid null VkShaderModule in Pipeline::ComputeCreateInfo");

        VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo{};
        vkPipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkPipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        vkPipelineShaderStageCreateInfo.module = createInfo.vkShaderModule;
        vkPipelineShaderStageCreateInfo.pName = createInfo.entryPoint.c_str();

        VkComputePipelineCreateInfo vkComputePipelineCreateInfo{};
        vkComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        vkComputePipelineCreateInfo.stage = vkPipelineShaderStageCreateInfo;
        vkComputePipelineCreateInfo.layout = createInfo.vkPipelineLayout;

        const VkResult vkResult = vkCreateComputePipelines(mDevice.get(), VK_NULL_HANDLE, 1, &vkComputePipelineCreateInfo, nullptr, &mVkPipeline);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create compute VkPipeline. ", utility::translateErrorCode(vkResult).c_str());
    }

    Pipeline::~Pipeline()
    {
        vkDestroyPipeline(mDevice.get(), mVkPipeline, nullptr);
        mVkPipeline = VK_NULL_HANDLE;
    }

    Pipeline::Pipeline(Pipeline&& other) noexcept
        : mDevice{ other.mDevice }
        , mVkPipeline{ other.mVkPipeline }
        , mVkPipelineBindPoint{ other.mVkPipelineBindPoint }
    {
        other.mVkPipeline = VK_NULL_HANDLE;
    }
}
