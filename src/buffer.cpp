#include "buffer.hpp"

void Buffer::destroy(VmaAllocator& allocator) {
    vmaDestroyBuffer(allocator, buffer, allocation);
}

void Buffer::setData(const void* data) {
    memcpy(allocInfo.pMappedData, data, size);
}