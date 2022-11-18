#include "instanceData.hpp"

VkVertexInputBindingDescription InstanceData::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 1;
    bindingDescription.stride = sizeof(InstanceData);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 1> InstanceData::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

    attributeDescriptions[0].binding = 1;
    attributeDescriptions[0].location = 3;
    attributeDescriptions[0].format = VK_FORMAT_R32_SFLOAT;
    attributeDescriptions[0].offset = 0;

    return attributeDescriptions;
}