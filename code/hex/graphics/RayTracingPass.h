#pragma once

#include "core/common.h"

#include "vulkan/vkCommon.h"

#include "vulkan/ShaderInput.h"
#include "vulkan/ResourceTable.h"
#include "vulkan/Pipeline.h"

#include "PassCore.h"
#include "ImGUI.h"

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
        static constexpr uint32_t IMAGE_WIDTH = 400;
        static constexpr uint32_t IMAGE_HEIGHT = 300;
        static constexpr VkFormat SCENE_COLOR_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;

        struct Params
        {
            vec3f cameraCenter;
            vec3f pixel00Loc;
            vec3f pixelDeltaU;
            vec3f pixelDeltaV;

            uint32_t randomSeed;
            uint32_t samplesPerPixel;

            float quadZ;
        };

        RayTracingPassData();

        inline const Params& getParams() const { return mParams; }

        void setSceneColorPreview(const Device& device, const uint32_t frameIndex, VkImage vkImageSceneColor);

        inline const std::array<ImGuiTextureBinding, FRAMES_IN_FLIGHT>& getSceneColorBindings() const { return mSceneColorBindings; }

        void imGUIRender() override;
        void reset() override;

    private:
        void recalculate();

    private:
        uint32_t mRandomSeed = 0x1deadb0b;
        uint32_t mSamplesPerPixel = 1;

        vec3f mCameraCenter = vec3f(0.0f);
        float mFocalLength = 1.0f;

        float mQuadZ = 1.0f;

        Params mParams;

        ImTextureID mSceneColorTextureId = ImTextureID{};
        std::array<ImGuiTextureBinding, FRAMES_IN_FLIGHT> mSceneColorBindings;
    };

    class ResourceManager;

    class RayTracingPass : public DispatchPass
    {
    public:
        RayTracingPass(const Device& device, ResourceManager& resourceManager);
        ~RayTracingPass() override;

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

        const Device& mDevice;
    };
}
