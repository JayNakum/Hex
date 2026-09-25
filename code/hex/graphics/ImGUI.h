#pragma once

#include "core/common.h"

#include "vulkan/vkCommon.h"

#include "PassCore.h"

#include <imgui.h>

struct GLFWwindow;

namespace graphics
{
    class Instance;
    class PhysicalDevice;
    class Device;
    class CommandContext;
    class ResourceManager;

    class ImGuiPass : public GraphicsPass
    {
    public:
        static void registerWindow(std::function<void()> callback);

        static void matrix(const char* label, mat4f& matrix);
        static void matrixInput(const char* label, mat4f& matrix);

        ImGuiPass(const Instance& instance, const PhysicalDevice& physicalDevice, const Device& device, VkQueue vkQueueGraphics, const uint32_t graphicsFamilyIndex, const VkFormat vkFormatColor, const uint32_t imageCount, GLFWwindow* glfwWindow);
        ~ImGuiPass() override;

        ImGuiPass(const ImGuiPass&) = delete;
        ImGuiPass& operator=(const ImGuiPass&) = delete;

        ImGuiPass(ImGuiPass&&) = delete;
        ImGuiPass& operator=(ImGuiPass&&) = delete;

        void copyDataToGPU(CommandContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;
        void getInputOutputs(CommandContext& context, ResourceManager& resourceManager, FrameConstants frame) override;

        void render(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame) override;

    private:
        static std::mutex msCallbackMutex;
        static std::vector<std::function<void()>> msRenderCallbacks;
    };
}
