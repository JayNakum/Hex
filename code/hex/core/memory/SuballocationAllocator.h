#pragma once

#include "core/common.h"

namespace memory
{
    class SuballocationAllocator
    {
    private:
        struct FreeBlock { uint64_t offset; size_t size; };

    public:
        SuballocationAllocator(const size_t totalSizeInBytes, const uint64_t offset = 0)
            : mTotalSizeInBytes(totalSizeInBytes),
            mOffset(offset),
            mNextAllocationOffset(mOffset)
        {
        }

        SuballocationAllocator(const SuballocationAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes),
            mOffset(copy.mOffset),
            mNextAllocationOffset(copy.mNextAllocationOffset)
        {
        }

        inline SuballocationAllocator& operator=(const SuballocationAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mOffset = rhs.mOffset;
                mNextAllocationOffset = rhs.mNextAllocationOffset;

                mFreeList = rhs.mFreeList;
            }
            return *this;
        }

        inline size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline size_t getFragmentedSize() const
        {
            size_t fragmentedSize = 0;
            for (const FreeBlock& block : mFreeList)
            {
                fragmentedSize += block.size;
            }
            return fragmentedSize;
        }

        inline size_t getUnallocatedSize() const
        {
            return (mOffset + mTotalSizeInBytes) - mNextAllocationOffset;
        }

        inline uint64_t allocate(const size_t sizeInBytes)
        {
            uint64_t allocationOffset = 0x1deadb0b;

            coalesceFreeList();

            freeEmptyBlocks();

            for (size_t i = mFreeList.size(); i-- > 0;)
            {
                FreeBlock& block = mFreeList[i];

                if (block.size >= sizeInBytes)
                {
                    allocationOffset = block.offset;
                    block.offset += sizeInBytes;
                    block.size -= sizeInBytes;
                    return allocationOffset;
                }
            }

            HEX_SOFT_ASSERT(getFragmentedSize() < sizeInBytes,
                "There is ", sizeInBytes, " bytes space available but the memory is fragmented",
                ". Total size = ", mTotalSizeInBytes,
                ". Fragmented size = ", getFragmentedSize(),
                ". Unallocated Size = ", getUnallocatedSize(),
                ".");

            HEX_ASSERT(getUnallocatedSize() >= sizeInBytes,
                "Allocator out of memory trying to allocate ", sizeInBytes, " bytes",
                ". Total size = %d", mTotalSizeInBytes,
                ". Fragmented size = %d", getFragmentedSize(),
                ". Unallocated Size = %d", getUnallocatedSize(),
                ".");

            allocationOffset = mNextAllocationOffset;
            mNextAllocationOffset += sizeInBytes;
            return allocationOffset;
        }

        inline void deallocate(const uint64_t allocationOffset, const size_t sizeInBytes)
        {
            HEX_SOFT_ASSERT(sizeInBytes > 0, "Trying to free 0 bytes of memory. freeEmptyBlocks() will trim this block.");

            mFreeList.push_back({ allocationOffset, sizeInBytes });
        }

        inline void reset()
        {
            mFreeList.clear();
            mNextAllocationOffset = mOffset;
        }

    private:
        inline void coalesceFreeList()
        {
            if (mFreeList.empty())
            {
                return;
            }

            std::sort(mFreeList.begin(), mFreeList.end(), [](const FreeBlock& a, const FreeBlock& b) { return a.offset < b.offset; });

            size_t i = 0;
            for (size_t j = 1; j < mFreeList.size(); ++j)
            {
                FreeBlock& currentBlock = mFreeList[i];
                FreeBlock& nextBlock = mFreeList[j];

                if ((currentBlock.size > 0) &&
                    ((currentBlock.offset + currentBlock.size) == nextBlock.offset))
                {
                    currentBlock.size += nextBlock.size;
                    nextBlock.size = 0;
                }
                else
                {
                    i = j;
                }
            }
        }

        inline void freeEmptyBlocks()
        {
            std::sort(mFreeList.begin(), mFreeList.end(), [](FreeBlock& a, FreeBlock& b) { return a.size > b.size; });

            size_t newSize = mFreeList.size();
            while (newSize > 0 && mFreeList[newSize - 1].size == 0)
            {
                --newSize;
            }

            mFreeList.resize(newSize);
        }

    private:
        size_t mTotalSizeInBytes;
        uint64_t mOffset;
        uint64_t mNextAllocationOffset;
        std::vector<FreeBlock> mFreeList;
    };
}
