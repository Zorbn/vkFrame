#include "model.hpp"

Model Model::fromVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<uint16_t> indices, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device) {
    Model model;
    model.indexBuffer = Buffer::fromIndices(allocator, commands, graphicsQueue, device, indices);
    model.vertexBuffer = Buffer::fromVertices(allocator, commands, graphicsQueue, device, vertices);

    return model;
}

void Model::draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet) {
    VkBuffer vertexBuffers[] = {vertexBuffer.buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexBuffer.size), 1, 0, 0, 0);
}

void Model::destroy(VmaAllocator allocator) {
    vertexBuffer.destroy(allocator);
    indexBuffer.destroy(allocator);
}