#pragma once

#include "core/common.h"

#include "maths/maths.h"

namespace graphics
{
    class CommandContext;
}

namespace graphics
{
    inline constexpr uint32_t FRAMES_IN_FLIGHT = 3;
}

namespace graphics
{
    class ResourceManager;

    class GraphicsPass
    {
    public:
        virtual ~GraphicsPass() = default;

        virtual void resize(const uint32_t, const uint32_t, ResourceManager&) {}

        virtual bool needsCopyDataToGPU() const { return false; }
        virtual void copyDataToGPU(CommandContext& context, ResourceManager& resourceManager) = 0;
        virtual void freeGPUUploadData(ResourceManager& resourceManager) = 0;

        virtual void getInputOutputs(CommandContext& context, ResourceManager& resourceManager, const uint32_t frameIndex = 0) = 0;
    };

    class ComputePass
    {
    public:
        virtual ~ComputePass() = default;

        virtual void resize(const uint32_t, const uint32_t, ResourceManager&) {}

        virtual bool needsCopyDataToGPU() const { return false; }
        virtual void copyDataToGPU(CommandContext& context, ResourceManager& resourceManager) = 0;
        virtual void freeGPUUploadData(ResourceManager& resourceManager) = 0;

        virtual void getInputOutputs(CommandContext& context, ResourceManager& resourceManager, const uint32_t frameIndex = 0) = 0;
    };
}

namespace graphics
{
    struct alignas(16) FrameConstants
    {
        float totalTime = 0.0f;
        float deltaTime = 0.0f;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct alignas(16) PassConstants
    {
        mat4f view;
        mat4f invView;

        vec3f eyePosition;
        const uint32_t _passPadding0 = 0x1deadb0b;

        mat4f projection;
        mat4f invProjection;

        float nearZ = 0.0f;
        float farZ = 0.0f;
        const uint32_t _passPadding1 = 0x1deadb0b;
        const uint32_t _passPadding2 = 0x1deadb0b;

        mat4f viewProjection;
        mat4f invViewProjection;

        vec2f renderTargetSize;
        vec2f invRenderTargetSize;
        const uint32_t _passPadding[5] = { 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b };
    };

    class PassData
    {
    public:
        PassData();
        virtual ~PassData() = default;

        inline bool getIsDirty() const { return mIsDirty; }
        inline void setIsDirty(const bool value) { mIsDirty = value; }

        inline void setFrameConstants(const float totalTime = 0.0f, const float deltaTime = 0.0f)
        {
            mFrameConstants.totalTime = totalTime;
            mFrameConstants.deltaTime = deltaTime;
        }
        inline void setFrameConstants(const FrameConstants& frameConstants)
        {
            setFrameConstants(frameConstants.totalTime, frameConstants.deltaTime);
        }

        inline const FrameConstants getFrameConstants() const
        {
            return mFrameConstants;
        }

        inline void setViewMatrix(const mat4f viewMatrix = mat4f())
        {
            mPassConstants.view = viewMatrix;
            mPassConstants.invView = inverse(mPassConstants.view);

            mPassConstants.viewProjection = mPassConstants.projection * mPassConstants.view;
            mPassConstants.invViewProjection = inverse(mPassConstants.viewProjection);
        }

        inline void setEyePosition(const vec3f eyePosition = vec3f())
        {
            mPassConstants.eyePosition = eyePosition;
        }

        inline void setProjectionMatrix(const mat4f projectionMatrix = mat4f(), const float nearZ = 0.0f, const float farZ = 0.0f)
        {
            mPassConstants.projection = projectionMatrix;
            mPassConstants.invProjection = inverse(projectionMatrix);

            mPassConstants.nearZ = nearZ;
            mPassConstants.farZ = farZ;

            mPassConstants.viewProjection = mPassConstants.projection * mPassConstants.view;
            mPassConstants.invViewProjection = inverse(mPassConstants.viewProjection);
        }

        inline void setRenderTargetSize(const unsigned int width = 0, const unsigned int height = 0)
        {
            mPassConstants.renderTargetSize.x = static_cast<float>(width);
            mPassConstants.renderTargetSize.y = static_cast<float>(height);
            mPassConstants.invRenderTargetSize = 1 / mPassConstants.renderTargetSize;
        }

        inline void setPassConstants(const PassConstants& passConstants)
        {
            setViewMatrix(passConstants.view);
            setEyePosition(passConstants.eyePosition);
            setProjectionMatrix(passConstants.projection, passConstants.nearZ, passConstants.farZ);
            setRenderTargetSize(static_cast<unsigned int>(passConstants.renderTargetSize.x), static_cast<unsigned int>(passConstants.renderTargetSize.y));
        }

        inline const PassConstants getPassConstants() const
        {
            return mPassConstants;
        }

        virtual void imGUIRender();

        virtual void reset() = 0;

        void imGUICommonParams();
        void imGUISave();

    private:
        FrameConstants mFrameConstants;
        PassConstants mPassConstants;
        bool mIsDirty = true;
    };
}
