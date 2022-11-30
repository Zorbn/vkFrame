#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include <vector>

#include "commands.hpp"
#include "queueFamilyIndices.hpp"

class Buffer {
  public:
    static Buffer fromIndices(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue,
                              VkDevice device, const std::vector<uint16_t>& indices);
    static Buffer fromIndicesWithMax(VmaAllocator allocator, Commands& commands,
                                     VkQueue graphicsQueue, VkDevice device,
                                     const std::vector<uint16_t>& indices, const size_t maxIndices);

    template <typename T>
    static Buffer fromVertices(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue,
                               VkDevice device, const std::vector<T>& vertices) {
        return Buffer::fromVerticesWithMax(allocator, commands, graphicsQueue, device, vertices,
                                           vertices.size());
    }

    template <typename T>
    static Buffer fromVerticesWithMax(VmaAllocator allocator, Commands& commands,
                                      VkQueue graphicsQueue, VkDevice device,
                                      const std::vector<T>& vertices, const size_t maxVertices) {
        VkDeviceSize bufferByteSize = sizeof(vertices[0]) * maxVertices;

        Buffer stagingBuffer(allocator, bufferByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
        stagingBuffer.setData(vertices.data());

        Buffer vertexBuffer(allocator, bufferByteSize,
                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            false);

        stagingBuffer.copyTo(allocator, graphicsQueue, device, commands, vertexBuffer);
        stagingBuffer.destroy(allocator);

        return vertexBuffer;
    }

    Buffer();
    Buffer(VmaAllocator allocator, VkDeviceSize byteSize, VkBufferUsageFlags usage,
           bool cpuAccessable);
    void destroy(VmaAllocator& allocator);
    void setData(const void* data);
    void copyTo(VmaAllocator& allocator, VkQueue graphicsQueue, VkDevice device, Commands& commands,
                Buffer& dst);
    const VkBuffer& getBuffer();
    void map(VmaAllocator allocator, void** data);
    void unmap(VmaAllocator allocator);

  private:
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
    size_t byteSize;
};
