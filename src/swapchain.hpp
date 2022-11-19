#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <stdexcept>
#include <array>

#include "queueFamilyIndices.hpp"
#include "image.hpp"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain {
public:
    VkSwapchainKHR swapchain;
    std::vector<Image> images;
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;

    Image depthImage;
    VkImageView depthImageView;

    void create(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, int32_t windowWidth, int32_t windowHeight);
    void cleanup(VmaAllocator allocator, VkDevice device);
    void recreate(VmaAllocator allocator, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkRenderPass renderPass, int32_t windowWidth, int32_t windowHeight);
    void createFramebuffers(VkDevice device, VkRenderPass renderPass);
    void createDepthResources(VmaAllocator allocator, VkPhysicalDevice physicalDevice, VkDevice device);

    void createImageViews(VkDevice device);

    VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, int32_t windowWidth, int32_t windowHeight);
    SwapChainSupportDetails querySupport(VkPhysicalDevice device, VkSurfaceKHR surface);
};