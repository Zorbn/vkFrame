#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include "../deps/stb_image.h"

#include "buffer.hpp"

class Image {
public:
    VkImage image;
    VmaAllocation allocation;

    static Image createImage(VmaAllocator allocator, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    static Image createTextureImage(const std::string& image, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device);
    static VkSampler createTextureSampler(VkPhysicalDevice physicalDevice, VkDevice device);

    VkImageView createTextureView( VkDevice device);
    VkImageView createView(VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device);
    void transitionImageLayout(Commands& commands, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkQueue graphicsQueue, VkDevice device);
    void destroy(VmaAllocator allocator);
    void copyFromBuffer(Buffer& src, Commands& commands, VkQueue graphicsQueue, VkDevice device, uint32_t width, uint32_t height);
};