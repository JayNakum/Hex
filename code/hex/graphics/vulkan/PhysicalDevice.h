#pragma once

#include "vkCommon.h"

namespace graphics
{
    class Instance;

    class PhysicalDevice
    {
    public:
        struct SurfaceSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        PhysicalDevice(const Instance& instance);
        ~PhysicalDevice();

        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice& operator=(const PhysicalDevice&) = delete;

        PhysicalDevice(PhysicalDevice&& other) noexcept;
        PhysicalDevice& operator=(PhysicalDevice&&) = delete;

        inline const VkPhysicalDevice& get() const { return mVkPhysicalDevice; }
        const std::vector<VkQueueFamilyProperties> getAllQueueFamilyProperties() const;
        uint32_t getQueueFamilyIndex(const VkQueueFlags& flags) const;

        uint32_t getGraphicsQueueFamilyIndex() const;
        uint32_t getQueueCount(const uint32_t familyIndex) const;

        bool checkSurfaceSupport(const uint32_t queueFamilyIndex, VkSurfaceKHR surface) const;
        const SurfaceSupportDetails querySurfaceSupportDetails(VkSurfaceKHR surface) const;

        uint32_t findMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags propertyFlags) const;

    private:
        int rateSuitability(const VkPhysicalDevice& device);

    private:
        const Instance& mInstance;

        VkPhysicalDevice mVkPhysicalDevice;
    };
}
