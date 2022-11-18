#pragma once

#include <cinttypes>
#include <optional>

#include "buffer.hpp"
#include "vertex.hpp"
#include "instanceData.hpp"

class Model {
public:
    Buffer vertexBuffer;
    Buffer indexBuffer;
    std::optional<Buffer> vertexStagingBuffer = std::nullopt;
    std::optional<Buffer> indexStagingBuffer = std::nullopt;
    Buffer instanceBuffer;
    Buffer instanceStagingBuffer;
    bool modifiable;
    size_t size;
    size_t instanceCount = 0;

    static Model fromVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<uint16_t> indices, const size_t maxInstances, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device);
    static Model fromVerticesAndIndicesModifiable(const std::vector<Vertex>& vertices, const std::vector<uint16_t> indices, const size_t maxVertices, const size_t maxIndices, const size_t maxInstances, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device);
    void draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet);
    void update(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices, Commands& commands, VmaAllocator allocator, VkQueue graphicsQueue, VkDevice device);
    void updateInstances(const std::vector<InstanceData>& instances, Commands& commands, VmaAllocator allocator, VkQueue graphicsQueue, VkDevice device);
    void destroy(VmaAllocator allocator);
};