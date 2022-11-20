#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <array>

#include "image.hpp"
#include "swapchain.hpp"

class RenderPass {
public:
    void create(VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator allocator, Swapchain& swapchain, bool enableDepth);
    void recreate(VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator allocator, Swapchain& swapchain);

    void begin(const uint32_t imageIndex, VkCommandBuffer commandBuffer, VkExtent2D extent, float clearColorR, float clearColorB, float clearColorG, float clearColorA);
    void end(VkCommandBuffer commandBuffer);

    VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

    const VkRenderPass& getRenderPass();
    const VkFramebuffer& getFramebuffer(const uint32_t imageIndex);

    void cleanup(VmaAllocator, VkDevice device);

private:
    void createImages(VkDevice device, Swapchain& swapchain);
    void createFramebuffers(VkDevice device, VkExtent2D extent);
    void createDepthResources(VmaAllocator allocator, VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent);
    void createImageViews(VkDevice device);
    void cleanupForRecreation(VmaAllocator allocator, VkDevice device);

    VkRenderPass renderPass;

    std::vector<Image> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;

    Image depthImage;
    VkImageView depthImageView;
    VkFormat imageFormat;
    bool depthEnabled;
};