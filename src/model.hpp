#pragma once

#include <cinttypes>
#include <optional>

#include "buffer.hpp"
#include "vertex.hpp"

template <typename T>
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

    static Model<T> fromVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<uint16_t> indices, const size_t maxInstances, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device) {
        Model model;
        model.modifiable = false;
        model.size = indices.size();

        model.indexBuffer = Buffer::fromIndices(allocator, commands, graphicsQueue, device, indices);
        model.vertexBuffer = Buffer::fromVertices(allocator, commands, graphicsQueue, device, vertices);

        size_t instanceByteSize = maxInstances * sizeof(T);
        model.instanceStagingBuffer = Buffer::create(allocator, instanceByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
        model.instanceBuffer = Buffer::create(allocator, instanceByteSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, false);

        return model;
    }

    static Model<T> fromVerticesAndIndicesModifiable(const std::vector<Vertex>& vertices, const std::vector<uint16_t> indices, const size_t maxVertices, const size_t maxIndices, const size_t maxInstances, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device) {
        Model model;
        model.modifiable = true;
        model.size = indices.size();

        std::pair<Buffer, Buffer> indexBuffers = Buffer::fromIndicesWithMax(allocator, commands, graphicsQueue, device, indices, maxIndices);
        model.indexStagingBuffer = indexBuffers.first;
        model.indexBuffer = indexBuffers.second;

        std::pair<Buffer, Buffer> vertexBuffers = Buffer::fromVerticesWithMax(allocator, commands, graphicsQueue, device, vertices, maxVertices);
        model.vertexStagingBuffer = vertexBuffers.first;
        model.vertexBuffer = vertexBuffers.second;

        size_t instanceByteSize = maxInstances * sizeof(CustomInstanceData);
        model.instanceStagingBuffer = Buffer::create(allocator, instanceByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
        model.instanceBuffer = Buffer::create(allocator, instanceByteSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, false);

        return model;
    }

    void draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet) {
        if (instanceCount < 1) return;

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
        vkCmdBindVertexBuffers(commandBuffer, 1, 1, &instanceBuffer.buffer, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(size), instanceCount, 0, 0, 0);
    }

    void update(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices, Commands& commands, VmaAllocator allocator, VkQueue graphicsQueue, VkDevice device) {
        if (!modifiable) return;

        size = indices.size();

        if (vertexStagingBuffer) {
            vertexStagingBuffer.value().setData(vertices.data());
            vertexStagingBuffer.value().copyTo(allocator, graphicsQueue, device, commands, vertexBuffer);
        }

        if (indexStagingBuffer) {
            indexStagingBuffer.value().setData(indices.data());
            indexStagingBuffer.value().copyTo(allocator, graphicsQueue, device, commands, indexBuffer);
        }
    }

    void updateInstances(const std::vector<T>& instances, Commands& commands, VmaAllocator allocator, VkQueue graphicsQueue, VkDevice device) {
        instanceCount = instances.size();
        instanceStagingBuffer.setData(instances.data());
        instanceStagingBuffer.copyTo(allocator, graphicsQueue, device, commands, instanceBuffer);
    }

    void destroy(VmaAllocator allocator) {
        vertexBuffer.destroy(allocator);
        indexBuffer.destroy(allocator);

        if (vertexStagingBuffer) {
            vertexStagingBuffer.value().destroy(allocator);
        }

        if (indexStagingBuffer) {
            indexStagingBuffer.value().destroy(allocator);
        }
    }
};