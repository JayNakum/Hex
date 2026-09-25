#include "RayTracingPass.h"

#include "ImGUI.h"

#include "vulkan/Device.h"
#include "vulkan/utility.h"

#include "vulkan/CommandContext.h"

#include "ResourceManager.h"

namespace graphics
{
    void RayTracingPassData::imGUIRender()
    {
        ImGui::Begin("Ray Tracing Pass Data");

        imGUICommonParams();
        imGUISave();

        ImGui::End();
    }

    void RayTracingPassData::reset()
    {
        mParams = Params{};
    }

    ResourceTable::Layout RayTracingPass::buildResourceTableLayout()
    {
        ResourceTable::Layout layout{};
        layout.vkShaderStageFlagsVisibility = VK_SHADER_STAGE_COMPUTE_BIT;
        layout.ranges = {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
        };
        return layout;
    }

    std::vector<VkPushConstantRange> RayTracingPass::buildPushConstantRanges()
    {
        VkPushConstantRange range{};
        range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        range.offset = 0;
        range.size = sizeof(FrameConstants);
        return { range };
    }

    Pipeline RayTracingPass::buildPipeline(const Device& device, const ShaderInput& shaderInput)
    {
        const VkShaderModule vkShaderModule = createShaderModule(device, "Shaders/RayTracing.spv");

        Pipeline::ComputeCreateInfo createInfo{};
        createInfo.vkShaderModule = vkShaderModule;
        createInfo.entryPoint = "computeMain";
        createInfo.vkPipelineLayout = shaderInput.getPipelineLayout(device);

        Pipeline pipeline{ device, createInfo };

        vkDestroyShaderModule(device.get(), vkShaderModule, nullptr);
        return pipeline;
    }

    RayTracingPass::RayTracingPass(const Device& device, ResourceManager& resourceManager)
        : mPassData{}
        , mResourceTable{ {
            ResourceTable{ device, resourceManager.descriptorAllocator(), buildResourceTableLayout() },
            ResourceTable{ device, resourceManager.descriptorAllocator(), buildResourceTableLayout() },
            ResourceTable{ device, resourceManager.descriptorAllocator(), buildResourceTableLayout() }
        } }
        , mShaderInput{ ShaderInput::CreateInfo{ { mResourceTable[0].getDescriptorSetLayout() }, buildPushConstantRanges() } }
        , mPipeline{ buildPipeline(device, mShaderInput) }
    {
        for (uint32_t frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT; ++frameIndex)
        {
            const std::string outputName = frameResourceID("UA_OutputColor", frameIndex);

            resourceManager.createRenderTexture(
                outputName,
                RayTracingPassData::IMAGE_WIDTH, RayTracingPassData::IMAGE_HEIGHT,
                RayTracingPassData::SCENE_COLOR_FORMAT,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                MemoryHeap::ResourceCategory::RenderTargetData);

            resourceManager.createImageView(outputName, ResourceManager::ImageViewCreateInfo{}, "RT_OutputColor");

            resourceManager.createBuffer(
                frameResourceID("RT_CB_PassConstants", frameIndex),
                ResourceManager::BufferCreateInfo{ sizeof(PassConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
                MemoryHeap::ResourceCategory::UploadPersistentData);

            resourceManager.createBuffer(
                frameResourceID("CB_RayTracingParams", frameIndex),
                ResourceManager::BufferCreateInfo{ sizeof(RayTracingPassData::Params), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
                MemoryHeap::ResourceCategory::UploadPersistentData);
        }
    }

    void RayTracingPass::resize(const uint32_t, const uint32_t, ResourceManager&)
    {
    }

    void RayTracingPass::copyDataToGPU(CommandContext&, ResourceManager&)
    {
    }

    void RayTracingPass::freeGPUUploadData(ResourceManager&)
    {
    }

    void RayTracingPass::getInputOutputs(CommandContext& context, ResourceManager& resourceManager, FrameConstants frame)
    {
        resourceManager.transitionImage(context, frameResourceID("UA_OutputColor", frame.index), ResourceStates::UnorderedAccess);
    }

    void RayTracingPass::dispatch(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame)
    {
        const std::string outputName = frameResourceID("UA_OutputColor", frame.index);
        const VkImageView vkImageViewTarget = resourceManager.getImageView(outputName, "RT_OutputColor");
        const VkExtent2D vkExtent2D = resourceManager.getExtent(outputName);

        const std::string passConstantsBufferName = frameResourceID("RT_CB_PassConstants", frame.index);
        const PassConstants passConstants = mPassData.getPassConstants();
        resourceManager.writeBuffer(passConstantsBufferName, &passConstants, sizeof(passConstants));

        const std::string rayTracingParamsBufferName = frameResourceID("CB_RayTracingParams", frame.index);
        const RayTracingPassData::Params params = mPassData.getParams();
        resourceManager.writeBuffer(rayTracingParamsBufferName, &params, sizeof(params));

        ResourceTable& resourceTable = mResourceTable[frame.index];
        resourceTable.setResource(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resourceManager.getBuffer(passConstantsBufferName));
        resourceTable.setResource(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, vkImageViewTarget, ResourceStates::UnorderedAccess);
        resourceTable.setResource(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resourceManager.getBuffer(rayTracingParamsBufferName));

        context.bindPipeline(mPipeline);

        frame.width = RayTracingPassData::IMAGE_WIDTH;
        frame.height = RayTracingPassData::IMAGE_HEIGHT;
        mShaderInput.setConstants(0, &frame, sizeof(frame));
        mShaderInput.setResourceTable(0, resourceTable);
        mShaderInput.finalize(context);

        context.dispatch2D(vkExtent2D.width, vkExtent2D.height, 32, 32);
    }
}
