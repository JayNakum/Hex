#pragma once

#include "core/common.h"

#include "vulkan/vkCommon.h"

#include "vulkan/ShaderInput.h"
#include "vulkan/ResourceTable.h"
#include "vulkan/Pipeline.h"

#include "PassCore.h"

namespace graphics
{
    class Device;
    class CommandContext;
}

namespace graphics
{
    class RayTracingPassData : public PassData
    {
    public:
        static constexpr uint32_t IMAGE_WIDTH = 3840;
        static constexpr uint32_t IMAGE_HEIGHT = 2160;
        static constexpr VkFormat SCENE_COLOR_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;

        struct Params
        {

        };

        inline const Params& getParams() const { return mParams; }

        void imGUIRender() override;
        void reset() override;

    private:
        Params mParams;
    };

    class ResourceManager;

    class RayTracingPass : public ComputePass
    {
    public:
        RayTracingPass(const Device& device, ResourceManager& resourceManager);

        void resize(const uint32_t width, const uint32_t height, ResourceManager& resourceManager) override;

        void copyDataToGPU(CommandContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;
        void getInputOutputs(CommandContext& context, ResourceManager& resourceManager, FrameConstants frame) override;
        void dispatch(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame) override;

    private:
        static ResourceTable::Layout buildResourceTableLayout();
        static std::vector<VkPushConstantRange> buildPushConstantRanges();
        static Pipeline buildPipeline(const Device& device, const ShaderInput& shaderInput);

    private:
        RayTracingPassData mPassData;

        std::array<ResourceTable, FRAMES_IN_FLIGHT> mResourceTable;
        ShaderInput mShaderInput;
        Pipeline mPipeline;
    };
}
