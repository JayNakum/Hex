#pragma once

#include "core/common.h"

namespace memory
{
    class RingAllocator
    {
    public:
        RingAllocator(const size_t totalSizeInBytes, const uint64_t offset = 0)
            : mTotalSizeInBytes(totalSizeInBytes),
            mOffset(offset),
            mNextAllocationOffset(mOffset)
        {
        }

        RingAllocator(const RingAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes),
            mOffset(copy.mOffset),
            mNextAllocationOffset(copy.mNextAllocationOffset)
        {
        }

        inline RingAllocator& operator=(const RingAllocator& rhs)
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
            const bool fitsInTail = canAllocate(sizeInBytes);

            const uint64_t fitMask = -static_cast<uint64_t>(fitsInTail);
            const uint64_t allocationOffset = (mNextAllocationOffset & fitMask) | (mOffset & ~fitMask);
            mNextAllocationOffset = allocationOffset + sizeInBytes;
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
