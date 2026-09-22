#pragma once

#include <deque>

#include "vkCommon.h"

#include "CommandContext.h"

namespace graphics
{
    class Device;

    class CommandContextPool
    {
    public:
        CommandContextPool(const Device& device, const CommandContext::CreateInfo& createInfo, const size_t maxContexts = 8);
        ~CommandContextPool() = default;

        CommandContextPool(const CommandContextPool&) = delete;
        CommandContextPool& operator=(const CommandContextPool&) = delete;

        CommandContext& requestContext();

    private:
        const Device& mDevice;
        CommandContext::CreateInfo mCreateInfo;
        size_t mMaxContexts;

        std::deque<CommandContext> mPool;
    };
}
