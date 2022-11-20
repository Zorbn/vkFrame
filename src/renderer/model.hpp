#pragma once

#include <cinttypes>

#include "buffer.hpp"
#include "vertex.hpp"

template <typename T> class Model {
public:
    static Model<T> fromVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<uint16_t> indices, const size_t maxInstances, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device) {
        Model model;
        model.size = indices.size();

        model.indexBuffer = Buffer::fromIndices(allocator, commands, graphicsQueue, device, indices);
        model.vertexBuffer = Buffer::fromVertices(allocator, commands, graphicsQueue, device, vertices);

        size_t instanceByteSize = maxInstances * sizeof(T);
        model.instanceStagingBuffer = Buffer(allocator, instanceByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
        model.instanceBuffer = Buffer(allocator, instanceByteSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, false);

        return model;
    }

    void draw(VkCommandBuffer commandBuffer) {
        if (instanceCount < 1) return;

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.getBuffer(), offsets);
        vkCmdBindVertexBuffers(commandBuffer, 1, 1, &instanceBuffer.getBuffer(), offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(size), instanceCount, 0, 0, 0);
    }

    void update(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices, Commands& commands, VmaAllocator allocator, VkQueue graphicsQueue, VkDevice device) {
        size = indices.size();

        vkDeviceWaitIdle(device);
        indexBuffer.destroy(allocator);
        vertexBuffer.destroy(allocator);
        indexBuffer = Buffer::fromIndices(allocator, commands, graphicsQueue, device, indices);
        vertexBuffer = Buffer::fromVertices(allocator, commands, graphicsQueue, device, vertices);
    }

    void updateInstances(const std::vector<T>& instances, Commands& commands, VmaAllocator allocator, VkQueue graphicsQueue, VkDevice device) {
        instanceCount = instances.size();
        instanceStagingBuffer.setData(instances.data());
        instanceStagingBuffer.copyTo(allocator, graphicsQueue, device, commands, instanceBuffer);
    }

    void destroy(VmaAllocator allocator) {
        vertexBuffer.destroy(allocator);
        indexBuffer.destroy(allocator);
        instanceStagingBuffer.destroy(allocator);
        instanceBuffer.destroy(allocator);
    }

private:
    Buffer vertexBuffer;
    Buffer indexBuffer;
    Buffer instanceBuffer;
    Buffer instanceStagingBuffer;
    size_t size;
    size_t instanceCount = 0;
};