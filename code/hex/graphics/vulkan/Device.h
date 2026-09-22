#pragma once

#include "vkCommon.h"

namespace graphics
{
    class Instance;
    class PhysicalDevice;

    class Device
    {
    public:
        struct DeviceQueueCreateInfo
        {
            uint32_t familyIndex = 0;
            uint32_t count = 1;
            float* priorities = nullptr;
        };

        struct CreateInfo
        {
            const PhysicalDevice& physicalDevice;

            size_t queueCount = 0;
            DeviceQueueCreateInfo* deviceQueueInfo = nullptr;
        };

        Device(const Instance& instance, const CreateInfo& createInfo);
        ~Device();

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        Device(Device&& other) noexcept;
        Device& operator=(Device&&) = delete;

        inline const VkDevice& get() const { return mVkDevice; }

        void waitIdle() const;

    private:
        VkDevice mVkDevice;
    };
}
