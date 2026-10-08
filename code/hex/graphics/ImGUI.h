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

    class ImGuiPass : public RenderPass
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

        void copyDataToGPU(CommandContext&, const QueueFamilies&, ResourceManager&) override {}
        void freeGPUUploadData(ResourceManager&) override {}
        void preRender(CommandContext&, const QueueFamilies&, ResourceManager&, FrameConstants) override {}

        void render(CommandContext& context, const ResourceManager& resourceManager, FrameConstants frame) override;
        void postRender(CommandContext&, const QueueFamilies&, ResourceManager&, FrameConstants) override {}

        ImTextureID createTextureRGBA(CommandContext& context, const std::string& id, const uint32_t width, const uint32_t height, const void* rgba8);
        void destroyTexture(const std::string& id);

    private:
        struct Texture
        {
            VkImage vkImage = VK_NULL_HANDLE;
            VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
            VkImageView vkImageView = VK_NULL_HANDLE;
            VkSampler vkSampler = VK_NULL_HANDLE;
            VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        };

        void destroyTexture(Texture& texture);

    private:
        static std::mutex msCallbackMutex;
        static std::vector<std::function<void()>> msRenderCallbacks;

        const PhysicalDevice& mPhysicalDevice;
        const Device& mDevice;

        std::unordered_map<std::string, Texture> mTextures;
        std::vector<std::pair<VkBuffer, VkDeviceMemory>> mStagingBuffersPendingFree;
    };
}
