#include "ImGUI.h"

#if defined(HEX_IMGUI_ENABLED)
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#endif

#include "vulkan/Instance.h"
#include "vulkan/PhysicalDevice.h"
#include "vulkan/Device.h"
#include "vulkan/CommandContext.h"

namespace graphics
{
    std::mutex ImGuiPass::msCallbackMutex;
    std::vector<std::function<void()>> ImGuiPass::msRenderCallbacks;

    void ImGuiPass::registerWindow(std::function<void()> callback)
    {
        std::lock_guard<std::mutex> scopedLockGuard(msCallbackMutex);
        msRenderCallbacks.push_back(std::move(callback));
    }

#if defined(HEX_IMGUI_ENABLED)

    void ImGuiPass::matrix(const char* label, mat4f& matrix)
    {
        if (ImGui::TreeNode(label))
        {
            ImGui::BeginTable(label, 4, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoHostExtendX);
            for (size_t i = 0; i < 4; ++i)
            {
                ImGui::TableNextRow();
                for (size_t j = 0; j < 4; ++j)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("%.3f", static_cast<double>(matrix[i][j]));
                }
            }
            ImGui::EndTable();
            ImGui::TreePop();
        }
    }

    void ImGuiPass::matrixInput(const char* label, mat4f& matrix)
    {
        if (ImGui::TreeNode(label))
        {
            ImGui::BeginTable(label, 4, ImGuiTableFlags_Borders);
            for (size_t i = 0; i < 4; ++i)
            {
                ImGui::TableNextRow();
                for (size_t j = 0; j < 4; ++j)
                {
                    ImGui::TableNextColumn();
                    ImGui::PushID(static_cast<int>(i * 4 + j));
                    ImGui::InputFloat("##val", &matrix[i][j]);
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
            ImGui::TreePop();
        }
    }

    ImGuiPass::ImGuiPass(const Instance& instance, const PhysicalDevice& physicalDevice, const Device& device, VkQueue vkQueueGraphics, const uint32_t graphicsFamilyIndex, const VkFormat vkFormatColor, const uint32_t imageCount, GLFWwindow* glfwWindow)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = HEXVULKANPROFILE_MIN_API_VERSION;
        initInfo.Instance = instance.get();
        initInfo.PhysicalDevice = physicalDevice.get();
        initInfo.Device = device.get();
        initInfo.QueueFamily = graphicsFamilyIndex;
        initInfo.Queue = vkQueueGraphics;
        initInfo.DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE;
        initInfo.MinImageCount = imageCount;
        initInfo.ImageCount = imageCount;
        initInfo.MinAllocationSize = 1024 * 1024;
        initInfo.UseDynamicRendering = true;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &vkFormatColor;

        ImGui_ImplVulkan_Init(&initInfo);
    }

    ImGuiPass::~ImGuiPass()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        std::lock_guard<std::mutex> scopedLockGuard(msCallbackMutex);
        msRenderCallbacks.clear();
    }

    void ImGuiPass::copyDataToGPU(CommandContext&, ResourceManager&)
    {
    }

    void ImGuiPass::freeGPUUploadData(ResourceManager&)
    {
    }

    void ImGuiPass::getInputOutputs(CommandContext&, ResourceManager&, const uint32_t)
    {
    }

    void ImGuiPass::render(CommandContext& context, const VkImageView vkImageViewTarget, const VkExtent2D vkExtent2D)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiID dockspaceId = ImGui::GetID("Hex_MainDockspace");

        ImGui::Begin("Hex", nullptr, ImGuiWindowFlags_None);
        ImGui::DockSpace(dockspaceId);

        {
            std::lock_guard<std::mutex> scopedLockGuard(msCallbackMutex);
            for (const std::function<void()>& callback : msRenderCallbacks)
            {
                ImGui::SetNextWindowDockID(dockspaceId, ImGuiCond_FirstUseEver);
                callback();
            }
        }

        ImGui::End();

        ImGui::Render();

        CommandContext::RenderingInfo renderingInfo{};
        renderingInfo.vkImageViewColor = vkImageViewTarget;
        renderingInfo.vkRect2DExtent = { { 0, 0 }, vkExtent2D };
        renderingInfo.clear = false;

        context.beginRendering(renderingInfo);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), context.get());

        context.endRendering();

        const ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

#else

    void ImGuiPass::matrix(const char*, mat4f&)
    {
    }

    void ImGuiPass::matrixInput(const char*, mat4f&)
    {
    }

    ImGuiPass::ImGuiPass(const Instance&, const PhysicalDevice&, const Device&, VkQueue, const uint32_t, const VkFormat, const uint32_t, GLFWwindow*)
    {
    }

    ImGuiPass::~ImGuiPass()
    {
        std::lock_guard<std::mutex> scopedLockGuard(msCallbackMutex);
        msRenderCallbacks.clear();
    }

    void ImGuiPass::copyDataToGPU(CommandContext&, ResourceManager&)
    {
    }

    void ImGuiPass::freeGPUUploadData(ResourceManager&)
    {
    }

    void ImGuiPass::getInputOutputs(CommandContext&, ResourceManager&, const uint32_t)
    {
    }

    void ImGuiPass::render(CommandContext&, const VkImageView, const VkExtent2D)
    {
    }

#endif
}
