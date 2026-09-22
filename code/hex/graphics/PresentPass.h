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
    class ResourceManager;

    class PresentPass : public GraphicsPass
    {
    public:
        PresentPass(const Device& device, ResourceManager& resourceManager, const VkFormat colorFormat);

        void copyDataToGPU(CommandContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;
        void getInputOutputs(CommandContext& context, ResourceManager& resourceManager, const uint32_t frameIndex = 0) override;
        void render(CommandContext& context, ResourceManager& resourceManager, const VkImageView vkImageViewTarget, const VkExtent2D& vkExtent2D, const uint32_t frameIndex);

    private:
        static ResourceTable::Layout buildSourceTableLayout();
        static Pipeline buildPipeline(const Device& device, const VkFormat colorFormat, const ShaderInput& shaderInput);

    private:
        std::array<ResourceTable, FRAMES_IN_FLIGHT> mSourceTable;
        ShaderInput mShaderInput;
        Pipeline mPipeline;
    };
}
