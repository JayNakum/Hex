#pragma once

#include "vkCommon.h"

namespace graphics
{
    class Instance
    {
    public:
        static constexpr size_t MAX_PROFILES = 2;
        static constexpr size_t MAX_PROFILE_BLOCKS = 4;

        struct CreateInfo
        {
            const char* applicationName = "Hex";
            std::array<uint32_t, 3> applicationVersion = { 0, 0, 0 };

            const char* engineName = "Hex";
            std::array<uint32_t, 3> engineVersion = { 0, 0, 0 };
        };

        struct ProfileInfo
        {
            uint32_t profileCount = 0;
            std::array<VpProfileProperties, MAX_PROFILES> profiles{};
            uint32_t blockCount = 0;
            std::array<VpBlockProperties, MAX_PROFILE_BLOCKS> blocks{};
        };

        Instance(const CreateInfo& createInfo);
        ~Instance();

        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;

        Instance(Instance&& other) noexcept;
        Instance& operator=(Instance&&) = delete;

        inline const CreateInfo& getCreateInfo() const { return mCreateInfo; }
        inline const ProfileInfo& getProfileInfo() const { return mProfileInfo; }
        inline const VkInstance& get() const { return mVkInstance; }

    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    private:
        const CreateInfo mCreateInfo;
        ProfileInfo mProfileInfo;
        VkInstance mVkInstance;

        VkDebugUtilsMessengerEXT mVkDebugMessenger;
    };
}
