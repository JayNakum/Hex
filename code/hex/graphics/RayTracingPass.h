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
            vec3f cameraCenter;
            const float _pad0 = 0.0f;
            vec3f pixel00Loc;
            const float _pad1 = 0.0f;
            vec3f pixelDeltaU;
            const float _pad2 = 0.0f;
            vec3f pixelDeltaV;
            const float _pad3 = 0.0f;
        };

        RayTracingPassData();

        inline const Params& getParams() const { return mParams; }

        void imGUIRender() override;
        void reset() override;

    private:
        void recalculate();

    private:
        vec3f mCameraCenter = vec3f(0.0f);
        float mFocalLength = 1.0f;

        Params mParams;
    };

    class ResourceManager;

    class RayTracingPass : public DispatchPass
    {
    public:
        RayTracingPass(const Device& device, ResourceManager& resourceManager);

        void resize(const uint32_t, const uint32_t, ResourceManager&) override {}

        bool needsCopyDataToGPU() const override;
        void copyDataToGPU(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;
        void preDispatch(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame) override;
        void dispatch(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame) override;
        void postDispatch(CommandContext& context, const QueueFamilies& queueFamilies, ResourceManager& resourceManager, FrameConstants frame) override;

    private:
        void createGPUResources(const uint32_t frameIndex, ResourceManager& resourceManager);

    private:
        static ResourceTable::Layout buildResourceTableLayout();
        static std::vector<VkPushConstantRange> buildPushConstantRanges();
        static Pipeline buildPipeline(const Device& device, const ShaderInput& shaderInput);
        static VkDescriptorSetLayout createResourceTables(ResourceManager& resourceManager);

    private:
        RayTracingPassData mPassData;

        ShaderInput mShaderInput;
        Pipeline mPipeline;

        std::array<bool, FRAMES_IN_FLIGHT> mParamsAcquirePending;
    };
}
