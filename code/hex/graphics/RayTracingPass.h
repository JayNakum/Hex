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
        RayTracingPass(const Device& device, ResourceManager& resourceManager, const uint32_t width, const uint32_t height);

        void resize(const uint32_t width, const uint32_t height, ResourceManager& resourceManager) override;

        void copyDataToGPU(CommandContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;
        void getInputOutputs(CommandContext& context, ResourceManager& resourceManager, const uint32_t frameIndex = 0) override;
        void dispatch(CommandContext& context, ResourceManager& resourceManager, const FrameConstants& frame, const uint32_t frameIndex);

    private:
        static ResourceTable::Layout buildResourceTableLayout();
        static std::vector<VkPushConstantRange> buildPushConstantRanges();
        static Pipeline buildPipeline(const Device& device, const ShaderInput& shaderInput);

    private:
        static constexpr VkFormat SCENE_COLOR_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;

        RayTracingPassData mPassData;

        std::array<ResourceTable, FRAMES_IN_FLIGHT> mResourceTable;
        ShaderInput mShaderInput;
        Pipeline mPipeline;
    };
}
