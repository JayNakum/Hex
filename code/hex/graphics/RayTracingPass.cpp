#include "RayTracingPass.h"

#include "ImGUI.h"

#include "vulkan/Device.h"
#include "vulkan/utility.h"

#include "vulkan/CommandContext.h"

#include "ResourceManager.h"

namespace graphics
{
    RayTracingPassData::RayTracingPassData()
        : mParams{}
        , mSceneColorBindings{}
    {
        recalculate();
    }

    void RayTracingPassData::recalculate()
    {
        mParams.randomSeed = mRandomSeed;
        mParams.samplesPerPixel = mSamplesPerPixel;

        mParams.quadZ = mQuadZ;

        const float viewportHeight = 2.0f;
        const float aspectRatio = static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);
        const float viewportWidth = viewportHeight * aspectRatio;

        const vec3f viewportU = vec3f(viewportWidth, 0.0f, 0.0f);
        const vec3f viewportV = vec3f(0.0f, -viewportHeight, 0.0f);

        const vec3f pixelDeltaU = viewportU / static_cast<float>(IMAGE_WIDTH);
        const vec3f pixelDeltaV = viewportV / static_cast<float>(IMAGE_HEIGHT);

        const vec3f viewportUpperLeft = mCameraCenter - vec3f(0.0f, 0.0f, mFocalLength) - (viewportU * 0.5f) - (viewportV * 0.5f);
        const vec3f pixel00Loc = viewportUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV);

        mParams.cameraCenter = mCameraCenter;
        mParams.pixel00Loc = pixel00Loc;
        mParams.pixelDeltaU = pixelDeltaU;
        mParams.pixelDeltaV = pixelDeltaV;
    }

    void RayTracingPassData::setSceneColorPreview(const Device& device, const uint32_t frameIndex, VkImage vkImageSceneColor)
    {
        ImGuiTextureBinding& sceneColorBinding = mSceneColorBindings[frameIndex];
        if (sceneColorBinding.vkDescriptorSet == VK_NULL_HANDLE)
        {
            sceneColorBinding = ImGuiPass::bind(device, ImGuiPass::getSampler(ImGuiPass::Sampler::LinearClamp), vkImageSceneColor, SCENE_COLOR_FORMAT, ResourceStates::UnorderedAccess);
        }

        mSceneColorTextureId = sceneColorBinding.textureId;
    }

    void RayTracingPassData::imGUIRender()
    {
        ImGui::Begin("Ray Tracing Pass Data");

        imGUICommonParams();

        if (ImGui::CollapsingHeader("Ray Tracing Params"))
        {
            bool changed = false;
            changed |= ImGui::InputInt("Random Seed", reinterpret_cast<int*>(&mRandomSeed));
            changed |= ImGui::InputInt("Samples Per Pixel", reinterpret_cast<int*>(&mSamplesPerPixel));
            changed |= ImGui::DragFloat3("Camera Center", reinterpret_cast<float*>(&mCameraCenter));
            changed |= ImGui::DragFloat("Focal Length", &mFocalLength);

            changed |= ImGui::SliderFloat("Quad Position", &mQuadZ, 0.0f, 80.0f);

            if (changed)
            {
                recalculate();
            }

            ImGui::InputFloat3("Pixel00 Loc", reinterpret_cast<float*>(&mParams.pixel00Loc), "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat3("Pixel Delta U", reinterpret_cast<float*>(&mParams.pixelDeltaU), "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat3("Pixel Delta V", reinterpret_cast<float*>(&mParams.pixelDeltaV), "%.3f", ImGuiInputTextFlags_ReadOnly);
        }

        if (ImGui::CollapsingHeader("Scene Color", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (mSceneColorTextureId != ImTextureID{})
            {
                ImGui::Image(mSceneColorTextureId, ImVec2{ static_cast<float>(IMAGE_WIDTH) / 8.0f, static_cast<float>(IMAGE_HEIGHT) / 8.0f });
            }
        }

        imGUISave();

        ImGui::End();
    }

    void RayTracingPassData::reset()
    {
        mRandomSeed = 0x1deadb0b;
        mSamplesPerPixel = 1;
        mQuadZ = 1.0f;

        mCameraCenter = vec3f(0.0f);
        mFocalLength = 1.0f;
        recalculate();
    }

	ResourceTable::Layout RayTracingPass::buildResourceTableLayout()
	{
        ResourceTable::Layout layout{};
        layout.vkShaderStageFlagsVisibility = VK_SHADER_STAGE_COMPUTE_BIT;
        layout.ranges = {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
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

    VkDescriptorSetLayout RayTracingPass::createResourceTables(ResourceManager& resourceManager)
    {
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        for (uint32_t frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT; ++frameIndex)
        {
            const ResourceTable& resourceTable = resourceManager.createResourceTable(frameResourceID("ResourceTable_RayTracing_DispatchPass", frameIndex), buildResourceTableLayout());
            vkDescriptorSetLayout = resourceTable.getDescriptorSetLayout();
        }
        return vkDescriptorSetLayout;
    }

    RayTracingPass::RayTracingPass(const Device& device, ResourceManager& resourceManager)
        : mPassData{}
        , mShaderInput{ ShaderInput::CreateInfo{ { createResourceTables(resourceManager) }, buildPushConstantRanges() } }
        , mPipeline{ buildPipeline(device, mShaderInput) }
        , mParamsAcquirePending{}
        , mDevice{ device }
    {
        for (uint32_t frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT; ++frameIndex)
        {
            createGPUResources(frameIndex, resourceManager);
        }
    }

    RayTracingPass::~RayTracingPass()
    {
        for (const ImGuiTextureBinding& binding : mPassData.getSceneColorBindings())
        {
            if (binding.vkImageView != VK_NULL_HANDLE)
            {
                vkDestroyImageView(mDevice.get(), binding.vkImageView, nullptr);
            }
        }
    }

    bool RayTracingPass::needsCopyDataToGPU() const
    {
        return mPassData.getIsDirty();
    }

    void RayTracingPass::copyDataToGPU(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager)
    {
        mPassData.setIsDirty(false);

        const RayTracingPassData::Params& params = mPassData.getParams();

        for (uint32_t frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT; ++frameIndex)
        {
            const std::string uploaderName = frameResourceID("Uploader_CB_RayTracingParams_RayTracing_DispatchPass", frameIndex);
            const std::string destinationName = frameResourceID("CB_RayTracingParams_RayTracing_DispatchPass", frameIndex);

            resourceManager.writeBuffer(uploaderName, &params, sizeof(params));
            context.copyBuffer(resourceManager.getBuffer(uploaderName), resourceManager.getBuffer(destinationName), sizeof(params));

            context.transitionBarrier(resourceManager.getBuffer(destinationName), ResourceStates::CopyDest, ResourceStates::ConstantBufferRead, queueFamilies.copy, queueFamilies.compute, sizeof(RayTracingPassData::Params));
            mParamsAcquirePending[frameIndex] = true;
        }
    }

    void RayTracingPass::freeGPUUploadData(ResourceManager& resourceManager)
    {
        for (uint32_t frameIndex = 0; frameIndex < FRAMES_IN_FLIGHT; ++frameIndex)
        {
            resourceManager.releaseBuffer(frameResourceID("Uploader_CB_RayTracingParams_RayTracing_DispatchPass", frameIndex));
        }
    }

    void RayTracingPass::preDispatch(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame)
    {
        const VkImage vkImageSceneColor = resourceManager.getImage(frameResourceID("UA_SceneColor_RayTracing_DispatchPass", frame.index));
        context.transitionBarrier(vkImageSceneColor, ResourceStates::PixelShaderResource, ResourceStates::UnorderedAccess, VK_IMAGE_ASPECT_COLOR_BIT, queueFamilies.graphics, queueFamilies.compute);

        if (mParamsAcquirePending[frame.index])
        {
            const std::string destinationName = frameResourceID("CB_RayTracingParams_RayTracing_DispatchPass", frame.index);
            context.transitionBarrier(resourceManager.getBuffer(destinationName), ResourceStates::CopyDest, ResourceStates::ConstantBufferRead, queueFamilies.copy, queueFamilies.compute, sizeof(RayTracingPassData::Params));
            mParamsAcquirePending[frame.index] = false;
        }
    }

    void RayTracingPass::postDispatch(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame)
    {
        const VkImage vkImageSceneColor = resourceManager.getImage(frameResourceID("UA_SceneColor_RayTracing_DispatchPass", frame.index));
        context.transitionBarrier(vkImageSceneColor, ResourceStates::UnorderedAccess, ResourceStates::PixelShaderResource, VK_IMAGE_ASPECT_COLOR_BIT, queueFamilies.compute, queueFamilies.graphics);

        mPassData.setSceneColorPreview(mDevice, frame.index, vkImageSceneColor);
    }

    void RayTracingPass::dispatch(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame)
    {
        const std::string outputName = frameResourceID("UA_SceneColor_RayTracing_DispatchPass", frame.index);
        const VkImageView vkImageViewTarget = resourceManager.getImageView(outputName, "SR_SceneColor");
        const VkExtent2D vkExtent2D = resourceManager.getExtent(outputName);

        const std::string passConstantsBufferName = frameResourceID("CB_CommonPassParams_RayTracing_DispatchPass", frame.index);
        const PassConstants passConstants = mPassData.getPassConstants();
        resourceManager.writeBuffer(passConstantsBufferName, &passConstants, sizeof(passConstants));

        const std::string rayTracingParamsBufferName = frameResourceID("CB_RayTracingParams_RayTracing_DispatchPass", frame.index);

        ResourceTable& resourceTable = resourceManager.getResourceTable(frameResourceID("ResourceTable_RayTracing_DispatchPass", frame.index));
        resourceTable.setResource(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resourceManager.getBuffer(passConstantsBufferName));
        resourceTable.setResource(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resourceManager.getBuffer(rayTracingParamsBufferName));
        resourceTable.setResource(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, vkImageViewTarget, ResourceStates::UnorderedAccess);

        context.bindPipeline(mPipeline);

        frame.width = RayTracingPassData::IMAGE_WIDTH;
        frame.height = RayTracingPassData::IMAGE_HEIGHT;
        mShaderInput.setConstants(0, &frame, sizeof(frame));
        mShaderInput.setResourceTable(0, resourceTable);
        mShaderInput.finalize(context);

        context.dispatch2D(vkExtent2D.width, vkExtent2D.height, 32, 32);
    }

	void RayTracingPass::createGPUResources(const uint32_t frameIndex, ResourceManager& resourceManager)
	{
        const std::string outputName = frameResourceID("UA_SceneColor_RayTracing_DispatchPass", frameIndex);

        resourceManager.createRenderTexture(
            outputName,
            RayTracingPassData::IMAGE_WIDTH, RayTracingPassData::IMAGE_HEIGHT,
            RayTracingPassData::SCENE_COLOR_FORMAT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            MemoryHeap::ResourceCategory::RenderTargetData);

        resourceManager.createImageView(outputName, ResourceManager::ImageViewCreateInfo{}, "SR_SceneColor");

        resourceManager.createBuffer(
            frameResourceID("CB_CommonPassParams_RayTracing_DispatchPass", frameIndex),
            ResourceManager::BufferCreateInfo{ sizeof(PassConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
            MemoryHeap::ResourceCategory::UploadPersistentData);

        resourceManager.createBuffer(
            frameResourceID("CB_RayTracingParams_RayTracing_DispatchPass", frameIndex),
            ResourceManager::BufferCreateInfo{ sizeof(RayTracingPassData::Params), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT },
            MemoryHeap::ResourceCategory::ConstantsData);

        resourceManager.createBuffer(
            frameResourceID("Uploader_CB_RayTracingParams_RayTracing_DispatchPass", frameIndex),
            ResourceManager::BufferCreateInfo{ sizeof(RayTracingPassData::Params), VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
            MemoryHeap::ResourceCategory::UploadData);
	}
}
