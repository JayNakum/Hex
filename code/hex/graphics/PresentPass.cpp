#include "PresentPass.h"

#include "ImGUI.h"

#include "vulkan/Device.h"
#include "vulkan/utility.h"

#include "vulkan/CommandContext.h"

#include "ResourceManager.h"

namespace graphics
{
    void PresentPassData::imGUIRender()
    {
        ImGui::Begin("Present Pass Data");

        imGUICommonParams();
        imGUISave();

        ImGui::End();
    }

    void PresentPassData::reset()
    {
        mParams = Params{};
    }

    ResourceTable::Layout PresentPass::buildSourceTableLayout(ResourceManager& resourceManager)
    {
        const VkSampler vkSamplerLinearClamp = resourceManager.createSampler("Sampler_LinearClamp", ResourceManager::SamplerCreateInfo{});

        ResourceTable::Layout layout{};
        layout.vkShaderStageFlagsVisibility = VK_SHADER_STAGE_FRAGMENT_BIT;
        layout.ranges = {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1 },
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1, vkSamplerLinearClamp },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
        };
        return layout;
    }

    std::vector<VkPushConstantRange> PresentPass::buildPushConstantRanges()
    {
        VkPushConstantRange range{};
        range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        range.offset = 0;
        range.size = sizeof(FrameConstants);
        return { range };
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

    VkDescriptorSetLayout PresentPass::createSourceTables(ResourceManager& resourceManager)
    {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        for (uint32_t frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT; ++frameIndex)
        {
            const ResourceTable& sourceTable = resourceManager.createResourceTable(frameResourceID("ResourceTable_Present_RenderPass", frameIndex), buildSourceTableLayout(resourceManager));
            vkDescriptorSetLayout = sourceTable.getDescriptorSetLayout();
        }
        return vkDescriptorSetLayout;
    }

    PresentPass::PresentPass(const Device& device, ResourceManager& resourceManager, const VkFormat colorFormat)
        : mPassData{}
        , mShaderInput{ ShaderInput::CreateInfo{ { createSourceTables(resourceManager) }, buildPushConstantRanges() } }
        , mPipeline{ buildPipeline(device, colorFormat, mShaderInput) }
    {
        for (uint32_t frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT; ++frameIndex)
        {
            resourceManager.createBuffer(
                frameResourceID("CB_CommonPassParams_Present_RenderPass", frameIndex),
                ResourceManager::BufferCreateInfo{ sizeof(PassConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
                MemoryHeap::ResourceCategory::UploadPersistentData);

            resourceManager.createBuffer(
                frameResourceID("CB_BlitParams_Present_RenderPass", frameIndex),
                ResourceManager::BufferCreateInfo{ sizeof(BlitParams), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
                MemoryHeap::ResourceCategory::UploadPersistentData);
        }
    }

    void PresentPass::preRender(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame)
    {
        const VkImage vkImageSceneColor = resourceManager.getImage(frameResourceID("UA_SceneColor_RayTracing_DispatchPass", frame.index));
        context.transitionBarrier(vkImageSceneColor, ResourceStates::UnorderedAccess, ResourceStates::PixelShaderResource, VK_IMAGE_ASPECT_COLOR_BIT, queueFamilies.compute, queueFamilies.graphics);
    }

    void PresentPass::postRender(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame)
    {
        const VkImage vkImageSceneColor = resourceManager.getImage(frameResourceID("UA_SceneColor_RayTracing_DispatchPass", frame.index));
        context.transitionBarrier(vkImageSceneColor, ResourceStates::PixelShaderResource, ResourceStates::UnorderedAccess, VK_IMAGE_ASPECT_COLOR_BIT, queueFamilies.graphics, queueFamilies.compute);
    }

    void PresentPass::render(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame)
    {
        const std::string sourceName = frameResourceID("UA_SceneColor_RayTracing_DispatchPass", frame.index);
        const VkImageView vkImageViewSource = resourceManager.getImageView(sourceName, "SR_SceneColor");
        const VkExtent2D vkExtent2DSource = resourceManager.getExtent(sourceName);

        const VkImageView vkImageViewTarget = resourceManager.getImageView("RT_BackBuffer_Renderer");
        const VkExtent2D vkExtent2DTarget = resourceManager.getExtent("RT_BackBuffer_Renderer");

        const BlitParams blitParams{
            static_cast<float>(vkExtent2DSource.width),
            static_cast<float>(vkExtent2DSource.height),
            static_cast<float>(vkExtent2DTarget.width),
            static_cast<float>(vkExtent2DTarget.height)
        };

        const std::string passConstantsBufferName = frameResourceID("CB_CommonPassParams_Present_RenderPass", frame.index);
        const PassConstants passConstants = mPassData.getPassConstants();
        resourceManager.writeBuffer(passConstantsBufferName, &passConstants, sizeof(passConstants));

        const std::string blitParamsBufferName = frameResourceID("CB_BlitParams_Present_RenderPass", frame.index);
        resourceManager.writeBuffer(blitParamsBufferName, &blitParams, sizeof(blitParams));

        ResourceTable& sourceTable = resourceManager.getResourceTable(frameResourceID("ResourceTable_Present_RenderPass", frame.index));
        sourceTable.setResource(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resourceManager.getBuffer(passConstantsBufferName));
        sourceTable.setResource(1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, vkImageViewSource, ResourceStates::PixelShaderResource);
        sourceTable.setResource(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resourceManager.getBuffer(blitParamsBufferName));

        CommandContext::RenderingInfo renderingInfo{};
        renderingInfo.vkImageViewColor = vkImageViewTarget;
        renderingInfo.vkRect2DExtent = { { 0, 0 }, vkExtent2DTarget };
        renderingInfo.clear = false;

        context.beginRendering(renderingInfo);

        context.setViewport(0.0f, 0.0f, static_cast<float>(vkExtent2DTarget.width), static_cast<float>(vkExtent2DTarget.height));
        context.setScissor(0, 0, vkExtent2DTarget.width, vkExtent2DTarget.height);

        context.bindPipeline(mPipeline);

        mShaderInput.setConstants(0, &frame, sizeof(frame));
        mShaderInput.setResourceTable(0, sourceTable);
        mShaderInput.finalize(context);

        context.draw(3);

        context.endRendering();
    }
}
