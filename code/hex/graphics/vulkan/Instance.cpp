#include "Instance.h"

#include "platform/win.h"

namespace graphics
{
    Instance::Instance(const CreateInfo& createInfo)
        : mCreateInfo{ createInfo }
        , mProfileInfo{}
        , mVkInstance{ VK_NULL_HANDLE }
        , mVkDebugMessenger{ VK_NULL_HANDLE }
    {
        const VkResult volkResult = volkInitialize();
        HEX_ASSERT(volkResult == VK_SUCCESS, "Failed to initialize volk. ", utility::translateErrorCode(volkResult).c_str());

        mProfileInfo.profileCount = 1;
        mProfileInfo.profiles[0] = VpProfileProperties{ HEXVULKANPROFILE_NAME, HEXVULKANPROFILE_SPEC_VERSION };

        VkApplicationInfo vkApplicationInfo{};
        vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkApplicationInfo.apiVersion = HEXVULKANPROFILE_MIN_API_VERSION;
        vkApplicationInfo.pApplicationName = mCreateInfo.applicationName;
        vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(
            mCreateInfo.applicationVersion[0],
            mCreateInfo.applicationVersion[1],
            mCreateInfo.applicationVersion[2]
        );

        vkApplicationInfo.pEngineName = mCreateInfo.engineName;
        vkApplicationInfo.engineVersion = VK_MAKE_VERSION(
            mCreateInfo.engineVersion[0],
            mCreateInfo.engineVersion[1],
            mCreateInfo.engineVersion[2]
        );

        VkInstanceCreateInfo vkInstanceCreateInfo{};
        vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if defined(HEX_DEBUG)
        constexpr bool needDebugMessenger = true;
#else
        constexpr bool needDebugMessenger = false;
#endif

        VkDebugUtilsMessengerCreateInfoEXT vkDebugUtilsMessengerCreateInfoEXT{};
        if (needDebugMessenger)
        {
            populateDebugMessengerCreateInfo(vkDebugUtilsMessengerCreateInfoEXT);
            vkInstanceCreateInfo.pNext = &vkDebugUtilsMessengerCreateInfoEXT;
        }

        vkInstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        vkInstanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

        VpInstanceCreateInfo vpInstanceCreateInfo{};
        vpInstanceCreateInfo.pCreateInfo = &vkInstanceCreateInfo;
        vpInstanceCreateInfo.enabledFullProfileCount = mProfileInfo.profileCount;
        vpInstanceCreateInfo.pEnabledFullProfiles = mProfileInfo.profiles.data();
        vpInstanceCreateInfo.enabledProfileBlockCount = mProfileInfo.blockCount;
        vpInstanceCreateInfo.pEnabledProfileBlocks = mProfileInfo.blocks.data();

        const VkResult vkResult = vpCreateInstance(&vpInstanceCreateInfo, nullptr, &mVkInstance);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkInstance. ", utility::translateErrorCode(vkResult).c_str());

        volkLoadInstance(mVkInstance);

        if (needDebugMessenger)
        {
            const VkResult debugMessengerResult = vkCreateDebugUtilsMessengerEXT(mVkInstance, &vkDebugUtilsMessengerCreateInfoEXT, nullptr, &mVkDebugMessenger);
            HEX_ASSERT(debugMessengerResult == VK_SUCCESS, "Failed to set up VkDebugUtilsMessengerEXT. ", utility::translateErrorCode(debugMessengerResult).c_str());
        }
    }

    Instance::~Instance()
    {
        if (mVkDebugMessenger != VK_NULL_HANDLE)
        {
            vkDestroyDebugUtilsMessengerEXT(mVkInstance, mVkDebugMessenger, nullptr);
            mVkDebugMessenger = VK_NULL_HANDLE;
        }

        if (mVkInstance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(mVkInstance, nullptr);
            mVkInstance = VK_NULL_HANDLE;
        }
    }

    Instance::Instance(Instance&& other) noexcept
        : mCreateInfo{ other.mCreateInfo }
        , mProfileInfo{ other.mProfileInfo }
        , mVkInstance{ other.mVkInstance }
        , mVkDebugMessenger{ other.mVkDebugMessenger }
    {
        other.mVkInstance = VK_NULL_HANDLE;
        other.mVkDebugMessenger = VK_NULL_HANDLE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Instance::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
        VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* /*pUserData*/)
    {
        HEX_LOG(VULKAN, pCallbackData->pMessage);
        return VK_FALSE;
    }

    void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }
}
