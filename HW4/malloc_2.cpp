#include <unistd.h>
#include <cmath>
#include <cassert>
#include <cstring>

struct MallocMetadata {
    size_t size;
    bool isFree;
    MallocMetadata *next;
    MallocMetadata *prev;
};

void *doAllocate(size_t size);

MallocMetadata *getStartOfBlockWithMetadata(size_t size, const void *newMemBlock);

void *allocateNewHead(size_t size);

void *findFreeBlock(size_t size);

MallocMetadata *getTail();

void *allocateNewBlock(size_t size);

MallocMetadata *getBlock(const void *p);

const int SBRK_ERROR_CODE = -1;

MallocMetadata *head = nullptr;

void *smalloc(size_t size) {
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
        block->isFree = true;
    }
}

void *srealloc(void *oldp, size_t size) {
    if (size == 0 || size > pow(10.0, 8.0)) {
        return nullptr;
    }

    MallocMetadata *oldBlock = getBlock(oldp);

    if (oldBlock != nullptr && oldBlock->size >= size) {
        ++oldBlock;
        return oldBlock;
    } else {
        void *newDataBlock = smalloc(size);

        if (newDataBlock != nullptr && oldBlock != nullptr) {
            MallocMetadata *dataBlock = oldBlock;
            ++dataBlock;
            memcpy(newDataBlock, dataBlock, oldBlock->size);

            sfree(dataBlock);
        }

        return newDataBlock;
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

void *allocateNewBlock(size_t size) {// If we got here then we need to allocate a new block
    void *newBlock = doAllocate(size + sizeof(MallocMetadata));

    MallocMetadata *startOfMemBlock = getStartOfBlockWithMetadata(size, newBlock);

    if (startOfMemBlock == nullptr) {
        return nullptr;
    } else {
        MallocMetadata *tail = getTail();

        assert(tail != nullptr);

        tail->next = startOfMemBlock;
        startOfMemBlock->prev = tail;

        startOfMemBlock++;

        return startOfMemBlock;
    }
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
    MallocMetadata *iterator = head;

    while (iterator != nullptr) {
        if (iterator->isFree && iterator->size >= size) {
            iterator->isFree = false;

            iterator++;

            return iterator;
        } else {
            iterator = iterator->next;
        }
    }

    return nullptr;
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

        return newMetadataBlock;
    } else {
        return nullptr;
    }
}

void *doAllocate(size_t size) {
    if (size == 0 || size > pow(10.0, 8.0)) {
        return nullptr;
    } else {
        void *memory = sbrk(size);

        return memory != (void *) SBRK_ERROR_CODE ? memory : nullptr;
    }
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

