#pragma once

#include "core/common.h"

namespace memory
{
    class LinearAllocator
    {
    public:
        LinearAllocator(const size_t totalSizeInBytes, const uint64_t offset = 0)
            : mTotalSizeInBytes(totalSizeInBytes),
            mOffset(offset),
            mNextAllocationOffset(mOffset)
        {
        }

        LinearAllocator(const LinearAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes),
            mOffset(copy.mOffset),
            mNextAllocationOffset(copy.mNextAllocationOffset)
        {
        }

        inline LinearAllocator& operator=(const LinearAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mOffset = rhs.mOffset;
                mNextAllocationOffset = rhs.mNextAllocationOffset;
            }
            return *this;
        }

        inline size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline size_t getRemainingSize() const
        {
            return (mOffset + mTotalSizeInBytes) - mNextAllocationOffset;
        }

        inline bool canAllocate(const size_t sizeInBytes) const
        {
            return sizeInBytes <= getRemainingSize();
        }

        inline uint64_t allocate(const size_t sizeInBytes)
        {
            HEX_ASSERT(canAllocate(sizeInBytes), "Allocator out of memory(", mTotalSizeInBytes, "). ",
                "Trying to allocate ", sizeInBytes, ". Remaining size = ", getRemainingSize());

            uint64_t allocationOffset = mNextAllocationOffset;
            mNextAllocationOffset += sizeInBytes;
            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = mOffset;
        }

    private:
        size_t mTotalSizeInBytes;
        uint64_t mOffset;
        uint64_t mNextAllocationOffset;
    };
}
