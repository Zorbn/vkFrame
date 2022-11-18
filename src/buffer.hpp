#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include <vector>

#include "vertex.hpp"
#include "commands.hpp"
#include "queueFamilyIndices.hpp"

struct Buffer {
public:
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
    size_t byteSize;

    void destroy(VmaAllocator& allocator);
    void setData(const void* data);
    static Buffer create(VmaAllocator allocator, VkDeviceSize byteSize, VkBufferUsageFlags usage, bool cpuAccessable);
    static Buffer fromIndices(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device, const std::vector<uint16_t>& indices);
    static Buffer fromVertices(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device, const std::vector<Vertex>& vertices);
    static Buffer fromIndicesWithMax(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device, const std::vector<uint16_t>& indices, const size_t maxIndices);
    static Buffer fromVerticesWithMax(VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device, const std::vector<Vertex>& vertices, const size_t maxVertices);
    void copyTo(VmaAllocator& allocator, VkQueue graphicsQueue, VkDevice device, Commands& commands, Buffer& dst);
    void copyToImage(Commands& commands, VkQueue graphicsQueue, VkDevice device, VkImage image, uint32_t width, uint32_t height);
};
