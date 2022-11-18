#pragma once

#include <cinttypes>

#include "buffer.hpp"

class Model {
public:
    Buffer vertexBuffer;
    Buffer indexBuffer;

    static Model fromVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<uint16_t> indices, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device);
    void draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet);
    void destroy(VmaAllocator allocator);
};