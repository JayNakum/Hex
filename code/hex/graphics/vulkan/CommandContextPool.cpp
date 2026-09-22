#include "CommandContextPool.h"

#include "Device.h"

namespace graphics
{
    CommandContextPool::CommandContextPool(const Device& device, const CommandContext::CreateInfo& createInfo, const size_t maxContexts)
        : mDevice{ device }
        , mCreateInfo{ createInfo }
        , mMaxContexts{ maxContexts }
        , mPool{}
    {
    }

    CommandContext& CommandContextPool::requestContext()
    {
        for (CommandContext& context : mPool)
        {
            if (vkGetFenceStatus(mDevice.get(), context.getFence()) == VK_SUCCESS)
            {
                return context;
            }
        }

        HEX_ASSERT(mPool.size() < mMaxContexts, "CommandContextPool exhausted its budget of ", mMaxContexts, " contexts.");

        mPool.emplace_back(mDevice, mCreateInfo);
        return mPool.back();
    }
}
