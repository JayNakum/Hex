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
    class PresentPassData : public PassData
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

    class PresentPass : public RenderPass
    {
    public:
        PresentPass(const Device& device, ResourceManager& resourceManager, const VkFormat colorFormat);

        void copyDataToGPU(CommandContext&, const QueueFamilies&, ResourceManager&) override {}
        void freeGPUUploadData(ResourceManager&) override {}
        void preRender(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame) override;
        void render(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame) override;
        void postRender(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame) override;

    private:
        struct BlitParams
        {
            float sourceWidth = 0.0f;
            float sourceHeight = 0.0f;
            float targetWidth = 0.0f;
            float targetHeight = 0.0f;
        };

        static ResourceTable::Layout buildSourceTableLayout(ResourceManager& resourceManager);
        static std::vector<VkPushConstantRange> buildPushConstantRanges();
        static Pipeline buildPipeline(const Device& device, const VkFormat colorFormat, const ShaderInput& shaderInput);
        static VkDescriptorSetLayout createSourceTables(ResourceManager& resourceManager);

    private:
        PresentPassData mPassData;

        ShaderInput mShaderInput;
        Pipeline mPipeline;
    };
}
