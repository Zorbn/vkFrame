#pragma once

#include <vulkan/vulkan.hpp>

struct InstanceData {
public:
    float x;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions();
};