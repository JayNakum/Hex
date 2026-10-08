#include "ImGUI.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "vulkan/Instance.h"
#include "vulkan/PhysicalDevice.h"
#include "vulkan/Device.h"
#include "vulkan/CommandContext.h"

#include "ResourceManager.h"

namespace graphics
{
    namespace
    {
        constexpr const char* UI_FONT_FILE = "JetBrainsMono-Regular.ttf";
        constexpr float UI_FONT_SIZE = 16.0f;

        constexpr uint8_t LOGO_GLYPH_0[8] = { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00 };
        constexpr uint8_t LOGO_GLYPH_x[8] = { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00 };
        constexpr uint8_t LOGO_GLYPH_H[8] = { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00 };
        constexpr uint8_t LOGO_GLYPH_e[8] = { 0x00, 0x00, 0x1E, 0x33, 0x3F, 0x03, 0x1E, 0x00 };

        const uint8_t* getLogoGlyph(const char c)
        {
            switch (c)
            {
            case '0': return LOGO_GLYPH_0;
            case 'x': return LOGO_GLYPH_x;
            case 'H': return LOGO_GLYPH_H;
            case 'e': return LOGO_GLYPH_e;
            default: return nullptr;
            }
        }

        ImVec2 drawPixelText(ImDrawList* vkDrawList, const ImVec2 origin, const char* text, const float pixelSize, const ImU32 color)
        {
            float cursorX = origin.x;

            for (const char* c = text; *c != '\0'; ++c)
            {
                const uint8_t* glyph = getLogoGlyph(*c);
                if (glyph != nullptr)
                {
                    for (uint32_t row = 0; row < 8; ++row)
                    {
                        const uint8_t rowBits = glyph[row];
                        for (uint32_t col = 0; col < 8; ++col)
                        {
                            if ((rowBits & (1 << col)) != 0)
                            {
                                const ImVec2 pixelMin{ cursorX + static_cast<float>(col) * pixelSize, origin.y + static_cast<float>(row) * pixelSize };
                                const ImVec2 pixelMax{ pixelMin.x + pixelSize, pixelMin.y + pixelSize };
                                vkDrawList->AddRectFilled(pixelMin, pixelMax, color);
                            }
                        }
                    }
                }

                cursorX += 9.0f * pixelSize;
            }

            return ImVec2{ cursorX - origin.x, 8.0f * pixelSize };
        }

    }

    std::mutex ImGuiPass::msCallbackMutex;
    std::vector<std::function<void()>> ImGuiPass::msRenderCallbacks;

    void ImGuiPass::registerWindow(std::function<void()> callback)
    {
        std::lock_guard<std::mutex> scopedLockGuard(msCallbackMutex);
        msRenderCallbacks.push_back(std::move(callback));
    }

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
        : mPhysicalDevice{ physicalDevice }
        , mDevice{ device }
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        const std::string fontPath = std::string(HEX_ASSETS_DIR) + "/fonts/" + UI_FONT_FILE;
        if (std::filesystem::exists(fontPath))
        {
            io.Fonts->AddFontFromFileTTF(fontPath.c_str(), UI_FONT_SIZE);
        }

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        const ImVec4 colorBackground{ 0.11f, 0.11f, 0.12f, 1.00f };
        const ImVec4 colorBackgroundLight{ 0.16f, 0.16f, 0.17f, 1.00f };
        const ImVec4 colorBackgroundLighter{ 0.22f, 0.22f, 0.23f, 1.00f };
        const ImVec4 colorAccent{ 0.10f, 0.64f, 0.98f, 1.00f };
        const ImVec4 colorAccentHovered{ 0.30f, 0.74f, 1.00f, 1.00f };
        const ImVec4 colorAccentActive{ 0.05f, 0.52f, 0.88f, 1.00f };
        const ImVec4 colorBorder{ 0.07f, 0.07f, 0.08f, 1.00f };

        colors[ImGuiCol_Text] = ImVec4{ 0.92f, 0.92f, 0.92f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.50f, 0.50f, 0.50f, 1.00f };
        colors[ImGuiCol_WindowBg] = colorBackground;
        colors[ImGuiCol_ChildBg] = colorBackground;
        colors[ImGuiCol_PopupBg] = colorBackgroundLight;
        colors[ImGuiCol_Border] = colorBorder;
        colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_FrameBg] = colorBackgroundLight;
        colors[ImGuiCol_FrameBgHovered] = colorBackgroundLighter;
        colors[ImGuiCol_FrameBgActive] = colorBackgroundLighter;
        colors[ImGuiCol_TitleBg] = colorBackground;
        colors[ImGuiCol_TitleBgActive] = colorBackground;
        colors[ImGuiCol_TitleBgCollapsed] = colorBackground;
        colors[ImGuiCol_MenuBarBg] = colorBackgroundLight;
        colors[ImGuiCol_ScrollbarBg] = colorBackground;
        colors[ImGuiCol_ScrollbarGrab] = colorBackgroundLighter;
        colors[ImGuiCol_ScrollbarGrabHovered] = colorAccentHovered;
        colors[ImGuiCol_ScrollbarGrabActive] = colorAccentActive;
        colors[ImGuiCol_CheckMark] = colorAccent;
        colors[ImGuiCol_SliderGrab] = colorAccent;
        colors[ImGuiCol_SliderGrabActive] = colorAccentActive;
        colors[ImGuiCol_Button] = colorBackgroundLighter;
        colors[ImGuiCol_ButtonHovered] = colorAccentHovered;
        colors[ImGuiCol_ButtonActive] = colorAccentActive;
        colors[ImGuiCol_Header] = colorBackgroundLighter;
        colors[ImGuiCol_HeaderHovered] = colorAccentHovered;
        colors[ImGuiCol_HeaderActive] = colorAccentActive;
        colors[ImGuiCol_Separator] = colorBorder;
        colors[ImGuiCol_SeparatorHovered] = colorAccent;
        colors[ImGuiCol_SeparatorActive] = colorAccentActive;
        colors[ImGuiCol_ResizeGrip] = colorBackgroundLighter;
        colors[ImGuiCol_ResizeGripHovered] = colorAccentHovered;
        colors[ImGuiCol_ResizeGripActive] = colorAccentActive;
        colors[ImGuiCol_Tab] = colorBackgroundLight;
        colors[ImGuiCol_TabHovered] = colorAccentHovered;
        colors[ImGuiCol_TabSelected] = colorAccentActive;
        colors[ImGuiCol_TabDimmed] = colorBackground;
        colors[ImGuiCol_TabDimmedSelected] = colorBackgroundLighter;
        colors[ImGuiCol_DockingPreview] = ImVec4{ colorAccent.x, colorAccent.y, colorAccent.z, 0.60f };
        colors[ImGuiCol_DockingEmptyBg] = colorBackground;
        colors[ImGuiCol_PlotLines] = colorAccent;
        colors[ImGuiCol_PlotLinesHovered] = colorAccentHovered;
        colors[ImGuiCol_PlotHistogram] = colorAccent;
        colors[ImGuiCol_PlotHistogramHovered] = colorAccentHovered;
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ colorAccent.x, colorAccent.y, colorAccent.z, 0.35f };
        colors[ImGuiCol_DragDropTarget] = colorAccent;
        colors[ImGuiCol_NavHighlight] = colorAccent;
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.70f };
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4{ 0.20f, 0.20f, 0.20f, 0.20f };
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.55f };

        style.WindowRounding = 4.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 3.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 3.0f;

        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;

        style.WindowPadding = ImVec2{ 10.0f, 10.0f };
        style.FramePadding = ImVec2{ 6.0f, 4.0f };
        style.ItemSpacing = ImVec2{ 8.0f, 6.0f };
        style.ItemInnerSpacing = ImVec2{ 6.0f, 4.0f };
        style.IndentSpacing = 18.0f;
        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 10.0f;

        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = HEXVULKANPROFILE_MIN_API_VERSION;
        initInfo.Instance = instance.get();
        initInfo.PhysicalDevice = physicalDevice.get();
        initInfo.Device = device.get();
        initInfo.QueueFamily = graphicsFamilyIndex;
        initInfo.Queue = vkQueueGraphics;
        initInfo.DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE + 64;
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
        for (std::pair<const std::string, Texture>& entry : mTextures)
        {
            destroyTexture(entry.second);
        }
        mTextures.clear();

        for (const std::pair<VkBuffer, VkDeviceMemory>& staging : mStagingBuffersPendingFree)
        {
            vkDestroyBuffer(mDevice.get(), staging.first, nullptr);
            vkFreeMemory(mDevice.get(), staging.second, nullptr);
        }
        mStagingBuffersPendingFree.clear();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        std::lock_guard<std::mutex> scopedLockGuard(msCallbackMutex);
        msRenderCallbacks.clear();
    }

    ImTextureID ImGuiPass::createTextureRGBA(CommandContext& context, const std::string& id, const uint32_t width, const uint32_t height, const void* rgba8)
    {
        const uint64_t byteSize = static_cast<uint64_t>(width) * height * 4;

        VkBufferCreateInfo vkBufferCreateInfoStaging{};
        vkBufferCreateInfoStaging.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vkBufferCreateInfoStaging.size = byteSize;
        vkBufferCreateInfoStaging.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        vkBufferCreateInfoStaging.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer vkBufferStaging = VK_NULL_HANDLE;
        VkResult vkResult = vkCreateBuffer(mDevice.get(), &vkBufferCreateInfoStaging, nullptr, &vkBufferStaging);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create staging VkBuffer. ", utility::translateErrorCode(vkResult).c_str());

        VkMemoryRequirements vkMemoryRequirementsStaging{};
        vkGetBufferMemoryRequirements(mDevice.get(), vkBufferStaging, &vkMemoryRequirementsStaging);

        VkMemoryAllocateInfo vkMemoryAllocateInfoStaging{};
        vkMemoryAllocateInfoStaging.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkMemoryAllocateInfoStaging.allocationSize = vkMemoryRequirementsStaging.size;
        vkMemoryAllocateInfoStaging.memoryTypeIndex = mPhysicalDevice.findMemoryType(vkMemoryRequirementsStaging.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkDeviceMemory vkDeviceMemoryStaging = VK_NULL_HANDLE;
        vkResult = vkAllocateMemory(mDevice.get(), &vkMemoryAllocateInfoStaging, nullptr, &vkDeviceMemoryStaging);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to allocate staging VkDeviceMemory. ", utility::translateErrorCode(vkResult).c_str());

        vkBindBufferMemory(mDevice.get(), vkBufferStaging, vkDeviceMemoryStaging, 0);

        void* mappedData = nullptr;
        vkMapMemory(mDevice.get(), vkDeviceMemoryStaging, 0, byteSize, 0, &mappedData);
        std::memcpy(mappedData, rgba8, byteSize);
        vkUnmapMemory(mDevice.get(), vkDeviceMemoryStaging);

        Texture texture{};

        VkImageCreateInfo vkImageCreateInfo{};
        vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        vkImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        vkImageCreateInfo.extent = { width, height, 1 };
        vkImageCreateInfo.mipLevels = 1;
        vkImageCreateInfo.arrayLayers = 1;
        vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        vkImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        vkImageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkResult = vkCreateImage(mDevice.get(), &vkImageCreateInfo, nullptr, &texture.vkImage);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkImage. ", utility::translateErrorCode(vkResult).c_str());

        VkMemoryRequirements vkMemoryRequirementsImage{};
        vkGetImageMemoryRequirements(mDevice.get(), texture.vkImage, &vkMemoryRequirementsImage);

        VkMemoryAllocateInfo vkMemoryAllocateInfoImage{};
        vkMemoryAllocateInfoImage.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkMemoryAllocateInfoImage.allocationSize = vkMemoryRequirementsImage.size;
        vkMemoryAllocateInfoImage.memoryTypeIndex = mPhysicalDevice.findMemoryType(vkMemoryRequirementsImage.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkResult = vkAllocateMemory(mDevice.get(), &vkMemoryAllocateInfoImage, nullptr, &texture.vkDeviceMemory);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to allocate image VkDeviceMemory. ", utility::translateErrorCode(vkResult).c_str());

        vkBindImageMemory(mDevice.get(), texture.vkImage, texture.vkDeviceMemory, 0);

        context.transitionBarrier(texture.vkImage, ResourceStates::Undefined, ResourceStates::CopyDest);
        context.copyBufferToImage(vkBufferStaging, texture.vkImage, width, height);
        context.transitionBarrier(texture.vkImage, ResourceStates::CopyDest, ResourceStates::PixelShaderResource);

        mStagingBuffersPendingFree.emplace_back(vkBufferStaging, vkDeviceMemoryStaging);

        VkImageViewCreateInfo vkImageViewCreateInfo{};
        vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vkImageViewCreateInfo.image = texture.vkImage;
        vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vkImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        vkImageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        vkImageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        vkResult = vkCreateImageView(mDevice.get(), &vkImageViewCreateInfo, nullptr, &texture.vkImageView);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkImageView. ", utility::translateErrorCode(vkResult).c_str());

        VkSamplerCreateInfo vkSamplerCreateInfo{};
        vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        vkSamplerCreateInfo.magFilter = VK_FILTER_NEAREST;
        vkSamplerCreateInfo.minFilter = VK_FILTER_NEAREST;
        vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        vkSamplerCreateInfo.maxLod = VK_LOD_CLAMP_NONE;
        vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

        vkResult = vkCreateSampler(mDevice.get(), &vkSamplerCreateInfo, nullptr, &texture.vkSampler);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkSampler. ", utility::translateErrorCode(vkResult).c_str());

        texture.vkDescriptorSet = ImGui_ImplVulkan_AddTexture(texture.vkSampler, texture.vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        const ImTextureID textureId = static_cast<ImTextureID>(reinterpret_cast<size_t>(texture.vkDescriptorSet));
        mTextures[id] = texture;
        return textureId;
    }

    void ImGuiPass::destroyTexture(Texture& texture)
    {
        if (texture.vkDescriptorSet != VK_NULL_HANDLE)
        {
            ImGui_ImplVulkan_RemoveTexture(texture.vkDescriptorSet);
        }
        if (texture.vkSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(mDevice.get(), texture.vkSampler, nullptr);
        }
        if (texture.vkImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(mDevice.get(), texture.vkImageView, nullptr);
        }
        if (texture.vkImage != VK_NULL_HANDLE)
        {
            vkDestroyImage(mDevice.get(), texture.vkImage, nullptr);
        }
        if (texture.vkDeviceMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(mDevice.get(), texture.vkDeviceMemory, nullptr);
        }

        texture = Texture{};
    }

    void ImGuiPass::destroyTexture(const std::string& id)
    {
        const std::unordered_map<std::string, Texture>::iterator it = mTextures.find(id);
        if (it == mTextures.end())
        {
            return;
        }

        destroyTexture(it->second);
        mTextures.erase(it);
    }

    void ImGuiPass::render(CommandContext& context, const ResourceManager& resourceManager, FrameConstants)
    {
        const VkImageView vkImageViewTarget = resourceManager.getImageView("RT_BackBuffer_Renderer");
        const VkExtent2D vkExtent2D = resourceManager.getExtent("RT_BackBuffer_Renderer");

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiID dockspaceId = ImGui::GetID("0x");

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

        const ImVec2 logoSize = drawPixelText(ImGui::GetWindowDrawList(), ImGui::GetCursorScreenPos(), "0xHex", 3.0f, IM_COL32(26, 163, 250, 255));
        ImGui::Dummy(logoSize);

#if defined(HEX_DEBUG)
        ImGui::Text("Config: Debug");
#elif defined(HEX_PROFILE)
        ImGui::Text("Config: Profile");
#else
        ImGui::Text("Config: Release");
#endif

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
}
