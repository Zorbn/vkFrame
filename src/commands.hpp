#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

#include "queueFamilyIndices.hpp"

class Commands {
public:
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkCommandBuffer beginSingleTimeCommands(VkQueue graphicsQueue, VkDevice device);
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkDevice device);

    void createCommandPool(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);

    void createCommandBuffers(VkDevice device, size_t maxFramesInFlight);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void destroy(VkDevice device);
};