#include "Device.h"

#include "Instance.h"
#include "PhysicalDevice.h"

namespace graphics
{
    Device::Device(const Instance& instance, const CreateInfo& createInfo)
        : mVkDevice{ VK_NULL_HANDLE }
    {
        std::vector<VkDeviceQueueCreateInfo> vkDeviceQueueCreateInfos;
        vkDeviceQueueCreateInfos.reserve(createInfo.queueCount);
        std::vector<std::vector<float>> defaultQueuePriorities(createInfo.queueCount);

        const float defaultQueuePriority = 1.0f;
        for (size_t i = 0; i < createInfo.queueCount; ++i)
        {
            VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo{};

            const DeviceQueueCreateInfo& deviceQueueCreateInfo = createInfo.deviceQueueInfo[i];

            vkDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            vkDeviceQueueCreateInfo.queueFamilyIndex = deviceQueueCreateInfo.familyIndex;
            vkDeviceQueueCreateInfo.queueCount = deviceQueueCreateInfo.count;

            vkDeviceQueueCreateInfo.pQueuePriorities = deviceQueueCreateInfo.priorities;
            if (deviceQueueCreateInfo.priorities == nullptr)
            {
                HEX_LOG(WARNING, "Device queue priorities not specified for DeviceQueueCreateInfo", i, ". Using defaultQueuePriority ", defaultQueuePriority);
                defaultQueuePriorities[i].assign(deviceQueueCreateInfo.count, defaultQueuePriority);
                vkDeviceQueueCreateInfo.pQueuePriorities = defaultQueuePriorities[i].data();
            }

            vkDeviceQueueCreateInfos.push_back(vkDeviceQueueCreateInfo);
        }

        VkDeviceCreateInfo vkDeviceCreateInfo{};
        vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        vkDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(vkDeviceQueueCreateInfos.size());
        vkDeviceCreateInfo.pQueueCreateInfos = vkDeviceQueueCreateInfos.data();
        vkDeviceCreateInfo.pEnabledFeatures = nullptr;
        vkDeviceCreateInfo.pNext = nullptr;

        VpDeviceCreateInfo vpDeviceCreateInfo{};
        vpDeviceCreateInfo.pCreateInfo = &vkDeviceCreateInfo;
        vpDeviceCreateInfo.enabledFullProfileCount = instance.getProfileInfo().profileCount;
        vpDeviceCreateInfo.pEnabledFullProfiles = instance.getProfileInfo().profiles.data();
        vpDeviceCreateInfo.enabledProfileBlockCount = instance.getProfileInfo().blockCount;
        vpDeviceCreateInfo.pEnabledProfileBlocks = instance.getProfileInfo().blocks.data();

        const VkResult vkResult = vpCreateDevice(createInfo.physicalDevice.get(), &vpDeviceCreateInfo, nullptr, &mVkDevice);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to create VkDevice. ", utility::translateErrorCode(vkResult).c_str());

        volkLoadDevice(mVkDevice);
    }

    Device::~Device()
    {
        vkDestroyDevice(mVkDevice, nullptr);
        mVkDevice = VK_NULL_HANDLE;
    }

    Device::Device(Device&& other) noexcept
        : mVkDevice{ other.mVkDevice }
    {
        other.mVkDevice = VK_NULL_HANDLE;
    }

    void Device::waitIdle() const
    {
        const VkResult vkResult = vkDeviceWaitIdle(mVkDevice);
        HEX_ASSERT(vkResult == VK_SUCCESS, "Failed to wait for VkDevice to become idle. ", utility::translateErrorCode(vkResult).c_str());
    }
}
