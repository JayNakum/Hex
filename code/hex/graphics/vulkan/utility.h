#pragma once

#include "core/common.h"

#include "vkCommon.h"

namespace graphics
{
    class Device;

    VkShaderModule createShaderModule(const Device& device, const std::string& spvPath);

    std::string frameResourceID(const std::string& baseName, const uint32_t frameIndex);
}
