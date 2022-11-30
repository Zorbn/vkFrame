#include "buffer.hpp"

Buffer::Buffer() {}

Buffer::Buffer(VmaAllocator allocator, VkDeviceSize byteSize, VkBufferUsageFlags usage,
               bool cpuAccessable) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = byteSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    if (cpuAccessable) {
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }

    this->byteSize = byteSize;

    if (vmaCreateBuffer(allocator, &bufferInfo, &allocCreateInfo, &buffer, &allocation,
                        &allocInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer!");
    }
}

void Buffer::copyTo(VmaAllocator& allocator, VkQueue graphicsQueue, VkDevice device,
                    Commands& commands, Buffer& dst) {
    VkCommandBuffer commandBuffer = commands.beginSingleTime(graphicsQueue, device);

    VkBufferCopy copyRegion{};
    copyRegion.size = dst.byteSize;
    vkCmdCopyBuffer(commandBuffer, buffer, dst.buffer, 1, &copyRegion);

    commands.endSingleTime(commandBuffer, graphicsQueue, device);
}

Buffer Buffer::fromIndices(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue,
                           VkDevice device, const std::vector<uint16_t>& indices) {
    return Buffer::fromIndicesWithMax(allocator, commands, graphicsQueue, device, indices,
                                      indices.size());
}

Buffer Buffer::fromIndicesWithMax(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue,
                                  VkDevice device, const std::vector<uint16_t>& indices,
                                  const size_t maxIndices) {
    VkDeviceSize bufferByteSize = sizeof(indices[0]) * maxIndices;

    Buffer stagingBuffer(allocator, bufferByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    stagingBuffer.setData(indices.data());

    Buffer indexBuffer(allocator, bufferByteSize,
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, false);

    stagingBuffer.copyTo(allocator, graphicsQueue, device, commands, indexBuffer);
    stagingBuffer.destroy(allocator);

    return indexBuffer;
}

const VkBuffer& Buffer::getBuffer() { return buffer; }

void Buffer::map(VmaAllocator allocator, void** data) { vmaMapMemory(allocator, allocation, data); }

void Buffer::unmap(VmaAllocator allocator) { vmaUnmapMemory(allocator, allocation); }

void Buffer::destroy(VmaAllocator& allocator) { vmaDestroyBuffer(allocator, buffer, allocation); }

void Buffer::setData(const void* data) { memcpy(allocInfo.pMappedData, data, byteSize); }