#pragma once

#include "core/common.h"

namespace memory
{
    class BlockAllocator
    {
        static constexpr unsigned int BLOCK_SIZE = 128;

    private:
        struct FreeBlock { uint64_t offset, count; };

    public:
        BlockAllocator(const size_t totalSizeInBytes, const size_t blockSize = BLOCK_SIZE, const uint64_t offset = 0)
            : mTotalSizeInBytes(((totalSizeInBytes + blockSize - 1) / blockSize)* blockSize),
            mBlockSize(blockSize),
            mOffset(offset),
            mNextAllocationOffset(mOffset)
        {
            HEX_SOFT_ASSERT((totalSizeInBytes % blockSize) == 0, "Invalid total size ", totalSizeInBytes, " and block size ", blockSize,
                ". Aligning up the total size to match block size. New Size = ", mTotalSizeInBytes);
        }

        BlockAllocator(const BlockAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes),
            mBlockSize(copy.mBlockSize),
            mOffset(copy.mOffset),
            mNextAllocationOffset(copy.mNextAllocationOffset)
        {
        }

        inline BlockAllocator& operator=(const BlockAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mBlockSize = rhs.mBlockSize;
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
                fragmentedSize += block.count;
            }
            return fragmentedSize * mBlockSize;
        }

        inline size_t getUnallocatedSize() const
        {
            return (mOffset + mTotalSizeInBytes) - mNextAllocationOffset;
        }

        inline uint64_t allocate(const size_t sizeInBytes)
        {
            HEX_SOFT_ASSERT((sizeInBytes % mBlockSize) == 0, "Invalid allocation size ", sizeInBytes, ". Aligning up the size to match block size.");

            uint64_t allocationOffset = 0x1deadb0b;

            const size_t blocksToAllocate = (sizeInBytes + mBlockSize - 1) / mBlockSize;

            for (size_t i = 0; i < mFreeList.size(); ++i)
            {
                FreeBlock& block = mFreeList[i];

                if (block.count == blocksToAllocate)
                {
                    allocationOffset = block.offset;
                    mFreeList.erase(mFreeList.begin() + static_cast<int>(i));
                    return allocationOffset;
                }
                else if (block.count > blocksToAllocate)
                {
                    allocationOffset = block.offset;
                    block.offset += blocksToAllocate * mBlockSize;
                    block.count -= blocksToAllocate;
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
            mNextAllocationOffset += (blocksToAllocate * mBlockSize);
            return allocationOffset;
        }

        inline void deallocate(const uint64_t allocationOffset, const size_t sizeInBytes)
        {
            const size_t blocksToFree = (sizeInBytes + mBlockSize - 1) / mBlockSize;

            size_t insertIndex = 0;
            for (; insertIndex < mFreeList.size(); ++insertIndex)
            {
                if (mFreeList[insertIndex].offset > allocationOffset)
                {
                    break;
                }
            }

            bool didCoalesce = false;
            if (insertIndex > 0)
            {
                FreeBlock& previousBlock = mFreeList[insertIndex - 1];
                if (previousBlock.offset + previousBlock.count * mBlockSize == allocationOffset)
                {
                    previousBlock.count += blocksToFree;

                    if (insertIndex < mFreeList.size())
                    {
                        FreeBlock& nextBlock = mFreeList[insertIndex];
                        if (allocationOffset + blocksToFree * mBlockSize == nextBlock.offset)
                        {
                            previousBlock.count += nextBlock.count;
                            mFreeList.erase(mFreeList.begin() + static_cast<int>(insertIndex));
                        }
                    }

                    didCoalesce = true;
                }
            }

            if (!didCoalesce && insertIndex < mFreeList.size())
            {
                FreeBlock& nextBlock = mFreeList[insertIndex];
                if (allocationOffset + blocksToFree * mBlockSize == nextBlock.offset)
                {
                    nextBlock.offset = allocationOffset;
                    nextBlock.count += blocksToFree;
                    didCoalesce = true;
                }
            }

            if (!didCoalesce)
            {
                mFreeList.insert(mFreeList.begin() + static_cast<int>(insertIndex), { allocationOffset, blocksToFree });
            }
        }

        inline void reset()
        {
            mFreeList.clear();
            mNextAllocationOffset = mOffset;
        }

    private:
        size_t mTotalSizeInBytes;
        size_t mBlockSize;
        uint64_t mOffset;
        uint64_t mNextAllocationOffset;
        std::vector<FreeBlock> mFreeList;
    };
}
