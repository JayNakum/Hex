#include "PhysicalDevice.h"

#include "Instance.h"

namespace graphics
{
    PhysicalDevice::PhysicalDevice(const Instance& instance)
        : mInstance{ instance }
        , mVkPhysicalDevice{ VK_NULL_HANDLE }
    {
        const VkInstance& vkInstance = mInstance.get();

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
        HEX_ASSERT(deviceCount, "Failed to find GPUs with vulkan support.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

        std::multimap<const int, VkPhysicalDevice> candidates;
        for (const VkPhysicalDevice& device : devices)
        {
            const int score = rateSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        HEX_ASSERT(candidates.rbegin()->first, "Failed to find a suitable GPU.");
        mVkPhysicalDevice = candidates.rbegin()->second;

        VkPhysicalDeviceProperties vkPhysicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(mVkPhysicalDevice, &vkPhysicalDeviceProperties);

        HEX_LOG(INFO, "Name: ", vkPhysicalDeviceProperties.deviceName);

        std::string type = "";
        switch (vkPhysicalDeviceProperties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        default:
            type = "Other";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            type = "Discrete GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            type = "Integrated GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            type = "Virtual GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            type = "CPU";
            break;
        }
        HEX_LOG(INFO, "Type: ", type.c_str());

        HEX_LOG(INFO, "API Version: ", VK_VERSION_MAJOR(vkPhysicalDeviceProperties.apiVersion), VK_VERSION_MINOR(vkPhysicalDeviceProperties.apiVersion), VK_VERSION_PATCH(vkPhysicalDeviceProperties.apiVersion));
        HEX_LOG(INFO, "Driver Version: ", VK_VERSION_MAJOR(vkPhysicalDeviceProperties.driverVersion), VK_VERSION_MINOR(vkPhysicalDeviceProperties.driverVersion), VK_VERSION_PATCH(vkPhysicalDeviceProperties.driverVersion));

        HEX_LOG(INFO, "Vendor ID: 0x", std::hex, vkPhysicalDeviceProperties.vendorID, std::dec);
    }

    PhysicalDevice::~PhysicalDevice()
    {
        mVkPhysicalDevice = VK_NULL_HANDLE;
    }

    PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
        : mInstance{ other.mInstance }
        , mVkPhysicalDevice{ other.mVkPhysicalDevice }
    {
        other.mVkPhysicalDevice = VK_NULL_HANDLE;
    }

    const std::vector<VkQueueFamilyProperties> PhysicalDevice::getAllQueueFamilyProperties() const
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &count, nullptr);

        HEX_ASSERT(count > 0, "mVkPhysicalDevice does not support any queue families.");

        std::vector<VkQueueFamilyProperties> queueFamilies(count);
        vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &count, queueFamilies.data());

        return queueFamilies;
    }

    uint32_t PhysicalDevice::getQueueFamilyIndex(const VkQueueFlags include, const VkQueueFlags exclude) const
    {
        const std::vector<VkQueueFamilyProperties> queueFamilies = getAllQueueFamilyProperties();

        for (size_t i = 0; i < queueFamilies.size(); ++i)
        {
            const VkQueueFlags flags = queueFamilies[i].queueFlags;
            if ((queueFamilies[i].queueCount > 0) && ((flags & include) == include) && ((flags & exclude) == 0))
            {
                return static_cast<uint32_t>(i);
            }
        }

        return UINT32_MAX;
    }

    bool PhysicalDevice::checkSurfaceSupport(const uint32_t queueFamilyIndex, VkSurfaceKHR surface) const
    {
        VkBool32 surfaceSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(mVkPhysicalDevice, queueFamilyIndex, surface, &surfaceSupport);
        return static_cast<bool>(surfaceSupport);
    }

    const PhysicalDevice::SurfaceSupportDetails PhysicalDevice::querySurfaceSupportDetails(VkSurfaceKHR surface) const
    {
        SurfaceSupportDetails details{};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysicalDevice, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    uint32_t PhysicalDevice::findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags propertyFlags) const
    {
        VkPhysicalDeviceMemoryProperties properties;
        vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &properties);

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && ((properties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags))
            {
                return i;
            }
        }

        HEX_ASSERT(false, "Unable to find appropriate memory type with filter ", typeFilter, " and flags ", propertyFlags);
        return UINT32_MAX;
    }

    int PhysicalDevice::rateSuitability(const VkPhysicalDevice& device)
    {
        const VkInstance& vkInstance = mInstance.get();

        for (size_t i = 0; i < mInstance.getProfileInfo().profileCount; ++i)
        {
            const VpProfileProperties& profile = mInstance.getProfileInfo().profiles[i];

            VkBool32 supported = VK_FALSE;
            const VkResult result = vpGetPhysicalDeviceProfileSupport(vkInstance, device, &profile, &supported);

            if (result != VK_SUCCESS || supported == VK_FALSE)
            {
                return 0;
            }
        }

        int score = 1;

        VkPhysicalDeviceProperties vkPhysicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(device, &vkPhysicalDeviceProperties);
        if (vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(device, &vkPhysicalDeviceMemoryProperties);
        for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryHeapCount; ++i)
        {
            if (vkPhysicalDeviceMemoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                score += static_cast<int>(vkPhysicalDeviceMemoryProperties.memoryHeaps[i].size / (1024 * 1024));
            }
        }

        return score;
    }
}
