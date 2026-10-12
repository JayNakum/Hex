#pragma once

#include "core/common.h"

#include "vulkan/vkCommon.h"
#include "vulkan/resourceState.h"

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

    struct ImGuiTextureBinding
    {
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        ImTextureID textureId = ImTextureID{};
    };

    struct ImGuiTexture
    {
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
        VkImageView vkImageView = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        ImTextureID textureId = ImTextureID{};
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct ImGuiTextureStaging
    {
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
    };

    class ImGuiPass : public RenderPass
    {
    public:
        enum class Sampler
        {
            LinearClamp,
            LinearRepeat,
            LinearMirror,
            NearestClamp,
            NearestRepeat,
            NearestMirror,
            Count
        };

        static void registerWindow(std::function<void()> callback);

        static void matrix(const char* label, mat4f& matrix);
        static void matrixInput(const char* label, mat4f& matrix);

        static VkSampler getSampler(const Sampler sampler);

        static ImGuiTextureBinding bind(const Device& device, VkSampler vkSampler, VkImage vkImage, const VkFormat vkFormat, const ResourceState& currentState = ResourceStates::PixelShaderResource);
        static void unbind(const Device& device, ImGuiTextureBinding& binding);

        static ImGuiTexture load(CommandContext& context, const PhysicalDevice& physicalDevice, const Device& device, VkSampler vkSampler, const std::string& filePath, ImGuiTextureStaging& outStaging);
        static void destroy(const Device& device, ImGuiTexture& texture);
        static void freeStaging(const Device& device, ImGuiTextureStaging& staging);

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

    private:
        static VkSampler createSampler(const Device& device, const VkFilter vkFilter, const VkSamplerAddressMode vkAddressMode);

    private:
        static std::mutex msCallbackMutex;
        static std::vector<std::function<void()>> msRenderCallbacks;

        static ImGuiPass* msInstance;

        const Device& mDevice;
        std::array<VkSampler, static_cast<size_t>(Sampler::Count)> mSamplers;
    };
}
