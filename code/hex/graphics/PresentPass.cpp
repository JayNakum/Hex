#include "PresentPass.h"

#include "vulkan/Device.h"
#include "vulkan/utility.h"

#include "vulkan/CommandContext.h"

#include "ResourceManager.h"

namespace graphics
{
    ResourceTable::Layout PresentPass::buildSourceTableLayout()
    {
        ResourceTable::Layout layout{};
        layout.vkShaderStageFlagsVisibility = VK_SHADER_STAGE_FRAGMENT_BIT;
        layout.ranges = { { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1 } };
        return layout;
    }

    Pipeline PresentPass::buildPipeline(const Device& device, const VkFormat colorFormat, const ShaderInput& shaderInput)
    {
        const VkShaderModule vkShaderModule = createShaderModule(device, "Shaders/Present.spv");

        Pipeline::GraphicsCreateInfo createInfo{};

        createInfo.shaderStages = {
            { VK_SHADER_STAGE_VERTEX_BIT, vkShaderModule, "vertMain" },
            { VK_SHADER_STAGE_FRAGMENT_BIT, vkShaderModule, "fragMain" }
        };

        createInfo.vkPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        createInfo.vkPolygonMode = VK_POLYGON_MODE_FILL;
        createInfo.vkCullModeFlags = VK_CULL_MODE_NONE;
        createInfo.vkFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

        createInfo.vkFormatsColorAttachment = { colorFormat };
        createInfo.colorBlendAttachments = { Pipeline::ColorBlendAttachment{} };
        createInfo.vkFormatDepthAttachment = VK_FORMAT_UNDEFINED;

        createInfo.depthTestEnable = false;
        createInfo.depthWriteEnable = false;

        createInfo.vkPipelineLayout = shaderInput.getPipelineLayout(device);

        Pipeline pipeline{ device, createInfo };

        vkDestroyShaderModule(device.get(), vkShaderModule, nullptr);
        return pipeline;
    }

    PresentPass::PresentPass(const Device& device, ResourceManager& resourceManager, const VkFormat colorFormat)
        : mSourceTable{ {
            ResourceTable{ device, resourceManager.descriptorAllocator(), buildSourceTableLayout() },
            ResourceTable{ device, resourceManager.descriptorAllocator(), buildSourceTableLayout() },
            ResourceTable{ device, resourceManager.descriptorAllocator(), buildSourceTableLayout() }
        } }
        , mShaderInput{ ShaderInput::CreateInfo{ { mSourceTable[0].getDescriptorSetLayout() }, {} } }
        , mPipeline{ buildPipeline(device, colorFormat, mShaderInput) }
    {
    }

    void PresentPass::copyDataToGPU(CommandContext&, ResourceManager&)
    {
    }

    void PresentPass::freeGPUUploadData(ResourceManager&)
    {
    }

    void PresentPass::getInputOutputs(CommandContext& context, ResourceManager& resourceManager, const uint32_t frameIndex)
    {
        resourceManager.transitionImage(context, frameResourceID("UA_OutputColor", frameIndex), ResourceStates::PixelShaderResource);
    }

    void PresentPass::render(CommandContext& context, ResourceManager& resourceManager, const VkImageView vkImageViewTarget, const VkExtent2D& vkExtent2D, const uint32_t frameIndex)
    {
        const VkImageView vkImageViewSource = resourceManager.getImageView(frameResourceID("UA_OutputColor", frameIndex), "RT_OutputColor");

        ResourceTable& sourceTable = mSourceTable[frameIndex];
        sourceTable.setResource(0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, vkImageViewSource, ResourceStates::PixelShaderResource);

        CommandContext::RenderingInfo renderingInfo{};
        renderingInfo.vkImageViewColor = vkImageViewTarget;
        renderingInfo.vkRect2DExtent = { { 0, 0 }, vkExtent2D };
        renderingInfo.clear = false;

        context.beginRendering(renderingInfo);

        context.setViewport(0.0f, 0.0f, static_cast<float>(vkExtent2D.width), static_cast<float>(vkExtent2D.height));
        context.setScissor(0, 0, vkExtent2D.width, vkExtent2D.height);

        context.bindPipeline(mPipeline);

        mShaderInput.setResourceTable(0, sourceTable);
        mShaderInput.finalize(context);

        context.draw(3);

        context.endRendering();
    }
}
