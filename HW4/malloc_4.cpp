#include <unistd.h>
#include <cmath>
#include <cassert>
#include <cstring>
#include <sys/mman.h>

struct MallocMetadata {
    size_t size;
    bool isFree;
    MallocMetadata *next;
    MallocMetadata *prev;
    bool isMMAP;
};

void *doAllocate(size_t size);

MallocMetadata *getStartOfBlockWithMetadata(size_t size, const void *newMemBlock);

void *allocateNewHead(size_t size);

void *findFreeBlock(size_t size);

MallocMetadata *getTail();

void *allocateNewBlock(size_t size);

MallocMetadata *getBlock(const void *p);

void insertMemBlock(MallocMetadata *startOfMemBlock);

size_t _size_meta_data();

void splitBlock(size_t size, MallocMetadata *block);

void removeBlockFromList(MallocMetadata *block);

void mergeWithAdjacentBlocks(MallocMetadata *block);

MallocMetadata *getWildernessBlock();

void mergeWithHigherAddress(MallocMetadata *block);

void mergeWithLowerAddress(MallocMetadata *block);

void *getNewDataBlock(size_t size, MallocMetadata *oldBlock);

void splitIfLargeEnough(size_t size, MallocMetadata *block);

void *resizeWildernessBlock(size_t size, MallocMetadata *wildernessBlock);

const int SBRK_ERROR_CODE = -1;

MallocMetadata *head = nullptr;

const size_t LARGE_ALLOCATION_SIZE = 128 * 1024;
const size_t LARGE_ENOUGH_SIZE = 128;

void *smalloc(size_t size) {
    size = size % 8 != 0 ? ((size >> 3) + 1) << 3 : size;

    if (size == 0 || size > pow(10.0, 8.0)) {
        return nullptr;
    } else if (head == nullptr) {
        return allocateNewHead(size);
    } else {
        void *reusedBlock = findFreeBlock(size);

        if (reusedBlock != nullptr) {
            return reusedBlock;
        } else {
            return allocateNewBlock(size);
        }
    }
}

void *scalloc(size_t num, size_t size) {
    void *block = smalloc(num * size);

    if (block != nullptr) {
        memset(block, 0, num * size);
    }

    return block;
}

void sfree(void *p) {
    MallocMetadata *block = getBlock(p);

    if (block != nullptr) {
        if (block->isMMAP) {
            removeBlockFromList(block);

            munmap(block, block->size + _size_meta_data());
        } else {
            block->isFree = true;

            mergeWithAdjacentBlocks(block);
        }
    }
}

void *srealloc(void *oldp, size_t size) {
    if (size == 0 || size > pow(10.0, 8.0)) {
        return nullptr;
    }

    size = size % 8 != 0 ? ((size >> 3) + 1) << 3 : size;

    MallocMetadata *oldBlock = getBlock(oldp);

    void *newDataBlock = getNewDataBlock(size, oldBlock);

    if (newDataBlock != nullptr && oldBlock != nullptr) {
        // trying to check if to split
        MallocMetadata *newMetadataBlock = getBlock(newDataBlock);

        assert(newMetadataBlock->size >= size);

        if (newMetadataBlock != oldBlock) {
            MallocMetadata *dataBlock = oldBlock;
            ++dataBlock;
            memmove(newDataBlock, dataBlock, oldBlock->size);

            sfree(dataBlock);
        }

        splitIfLargeEnough(size, newMetadataBlock);
    }

    return newDataBlock;
}

void splitIfLargeEnough(size_t size, MallocMetadata *block) {
    bool largeEnough = block->size - size - _size_meta_data() >= LARGE_ENOUGH_SIZE &&
                       block->size - size >=
                       LARGE_ENOUGH_SIZE; // This is done incase of an underflow and I don't want to cast sizes to integers

    if (largeEnough && !block->isMMAP) {
        splitBlock(size, block);
    }
}

void *getNewDataBlock(size_t size, MallocMetadata *oldBlock) {
    size_t lowerAdjacentBlockSize = oldBlock != nullptr && oldBlock->prev != nullptr && oldBlock->prev->isFree
                                    ? oldBlock->prev->size + _size_meta_data() : 0;
    size_t higherAdjacentBlockSize = oldBlock != nullptr && oldBlock->next != nullptr && oldBlock->next->isFree ?
                                     oldBlock->next->size + _size_meta_data() : 0;

    void *newDataBlock;

    if (oldBlock != nullptr && oldBlock->size >= size) {
        MallocMetadata *oldMetaData = oldBlock;
        oldMetaData++;

        newDataBlock = oldMetaData;
    } else if (oldBlock != nullptr && oldBlock->size + lowerAdjacentBlockSize >= size) {
        MallocMetadata *previous = oldBlock->prev;
        ++previous;

        newDataBlock = previous;

        mergeWithLowerAddress(oldBlock);
        oldBlock->prev->isFree = false;
    } else if (oldBlock != nullptr && oldBlock->size + higherAdjacentBlockSize >= size) {
        mergeWithHigherAddress(oldBlock);
        MallocMetadata *oldMetaData = oldBlock;
        oldMetaData++;

        newDataBlock = oldMetaData;
    } else if (oldBlock != nullptr && oldBlock->size + higherAdjacentBlockSize + lowerAdjacentBlockSize >= size) {
        MallocMetadata *previous = oldBlock->prev;
        ++previous;

        newDataBlock = previous;

        mergeWithAdjacentBlocks(oldBlock);
        oldBlock->prev->isFree = false;
    } else {
        if (oldBlock == getWildernessBlock()) {
            oldBlock->isFree = true; // Try to give this block the chance of wilderness enlargement
        }

        newDataBlock = smalloc(size);

        if (newDataBlock == nullptr) { //On the off chance sbrk failed
            oldBlock->isFree = false;
        }
    }

    return newDataBlock;
}


void mergeWithAdjacentBlocks(MallocMetadata *block) {
    mergeWithHigherAddress(block);

    mergeWithLowerAddress(block);
}

void mergeWithLowerAddress(MallocMetadata *block) {
    if (block->prev != nullptr && block->prev->isFree && !(block->prev->isMMAP)) {
        block->prev->size += block->size + _size_meta_data();
        removeBlockFromList(block);
    }
}

void mergeWithHigherAddress(MallocMetadata *block) {
    if (block->next != nullptr && block->next->isFree && !(block->next->isMMAP)) {
        block->size += block->next->size + _size_meta_data();
        removeBlockFromList(block->next);
    }
}

void removeBlockFromList(MallocMetadata *block) {
    if (block->prev != nullptr) {
        block->prev->next = block->next;
    } else {
        assert(block == head);
        head = block->next;
    }

    if (block->next != nullptr) {
        block->next->prev = block->prev;
    }
}

MallocMetadata *getBlock(const void *p) {
    if (p != nullptr) {
        MallocMetadata *iterator = head;

        while (iterator != nullptr) {
            MallocMetadata *temp = iterator;
            temp++;
            if ((temp) == p) {
                return iterator;
            } else {
                iterator = iterator->next;
            }
        }

    }

    return nullptr;
}

void *allocateNewBlock(size_t size) {
    MallocMetadata *wildernessBlock = getWildernessBlock();

    if (wildernessBlock != nullptr && wildernessBlock->isFree) {
        return resizeWildernessBlock(size, wildernessBlock);

    } else {
        void *newBlock = doAllocate(size + sizeof(MallocMetadata));

        MallocMetadata *startOfMemBlock = getStartOfBlockWithMetadata(size, newBlock);

        if (startOfMemBlock == nullptr) {
            return nullptr;
        } else {
            assert(head != nullptr);

            insertMemBlock(startOfMemBlock);

            startOfMemBlock++;

            return startOfMemBlock;
        }
    }
}

void *resizeWildernessBlock(size_t size, MallocMetadata *wildernessBlock) {
    assert(size > wildernessBlock->size);
    doAllocate(size - wildernessBlock->size);
    wildernessBlock->size = size;
    wildernessBlock->isFree = false;

    ++wildernessBlock;

    return wildernessBlock;
}

MallocMetadata *getWildernessBlock() {
    MallocMetadata *iterator = head;
    MallocMetadata *wildernessBlock = nullptr;

    while (iterator != nullptr) {
        if (!iterator->isMMAP) {
            wildernessBlock = iterator;
        }

        iterator = iterator->next;
    }
    return wildernessBlock;
}

void insertMemBlock(MallocMetadata *startOfMemBlock) {
    MallocMetadata *iterator = head;

    while (iterator != nullptr) {
        if (iterator > startOfMemBlock) {
            if (iterator->prev == nullptr) {
                //Head
                assert(head == iterator);
                head = startOfMemBlock;
                iterator->prev = startOfMemBlock;
                head->next = iterator;
            } else {
                MallocMetadata *previous = iterator->prev;
                previous->next = startOfMemBlock;
                startOfMemBlock->next = iterator;
                iterator->prev = startOfMemBlock;
                startOfMemBlock->prev = previous;
            }

            return;
        }

        iterator = iterator->next;
    }

    MallocMetadata *tail = getTail();
    tail->next = startOfMemBlock;
    startOfMemBlock->prev = tail;
}

MallocMetadata *getTail() {
    assert(head != nullptr);

    MallocMetadata *iterator = head;

    while (iterator->next != nullptr) {
        iterator = iterator->next;
    }

    return iterator;
}

void *findFreeBlock(size_t size) {
    bool isMMAP = size >= LARGE_ALLOCATION_SIZE;

    MallocMetadata *iterator = head;

    while (iterator != nullptr) {
        if (iterator->isFree && iterator->size >= size && iterator->isMMAP == isMMAP) {
            iterator->isFree = false;

            if (!isMMAP) {
                splitIfLargeEnough(size, iterator);
            }

            iterator++;

            return iterator;
        } else {
            iterator = iterator->next;
        }
    }

    return nullptr;
}

void splitBlock(size_t size, MallocMetadata *block) {
    char *newChunk = (char *) block;

    newChunk += _size_meta_data() + size;

    auto *splitedBlock = (MallocMetadata *) newChunk;

    splitedBlock->size = block->size - size - _size_meta_data();
    splitedBlock->isMMAP = false;
    splitedBlock->prev = nullptr;
    splitedBlock->next = nullptr;
    splitedBlock->isFree = true;

    insertMemBlock(splitedBlock);

    block->size = size;
}

void *allocateNewHead(size_t size) {
    void *newMemBlock = doAllocate(size + sizeof(MallocMetadata));

    MallocMetadata *startOfBlock = getStartOfBlockWithMetadata(size, newMemBlock);

    if (startOfBlock == nullptr) {
        return nullptr;
    } else {
        head = startOfBlock;

        startOfBlock++;

        return startOfBlock;
    }
}

MallocMetadata *getStartOfBlockWithMetadata(size_t size, const void *newMemBlock) {
    MallocMetadata *newMetadataBlock;
    if (newMemBlock != nullptr) {
        newMetadataBlock = (MallocMetadata *) newMemBlock;

        newMetadataBlock->size = size;
        newMetadataBlock->isFree = false;
        newMetadataBlock->next = nullptr;
        newMetadataBlock->prev = nullptr;
        newMetadataBlock->isMMAP = size >= LARGE_ALLOCATION_SIZE;

        return newMetadataBlock;
    } else {
        return nullptr;
    }
}

void *doAllocate(size_t size) {
    void *memory;

    if (size == 0 || size > pow(10.0, 8.0)) {
        return nullptr;
    } else if (size < LARGE_ALLOCATION_SIZE) {
        memory = sbrk(size);
    } else {
        assert(size >= LARGE_ALLOCATION_SIZE);

        memory = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);
    }

    return memory != (void *) SBRK_ERROR_CODE ? memory : nullptr;
}

size_t _num_free_blocks() {
    MallocMetadata *iterator = head;

    int numFree = 0;
    while (iterator != nullptr) {
        if (iterator->isFree) {
            ++numFree;
        }

        iterator = iterator->next;
    }

    return numFree;
}

size_t _num_free_bytes() {
    MallocMetadata *iterator = head;

    size_t numFreeBytes = 0;
    while (iterator != nullptr) {
        if (iterator->isFree) {
            numFreeBytes += iterator->size;
        }

        iterator = iterator->next;
    }

    return numFreeBytes;
}

size_t _num_allocated_blocks() {
    MallocMetadata *iterator = head;

    int numBlocks = 0;
    while (iterator != nullptr) {
        ++numBlocks;

        iterator = iterator->next;
    }

    return numBlocks;
}

size_t _num_allocated_bytes() {
    MallocMetadata *iterator = head;

    size_t numBytes = 0;
    while (iterator != nullptr) {
        numBytes += iterator->size;

        iterator = iterator->next;
    }

    return numBytes;
}

size_t _size_meta_data() {
    return sizeof(MallocMetadata);
}

size_t _num_meta_data_bytes() {
    return _num_allocated_blocks() * _size_meta_data();
}

