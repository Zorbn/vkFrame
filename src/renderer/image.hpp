#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include <stb_image.h>

#include "buffer.hpp"

class Image {
public:
    static Image createTexture(const std::string& image, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device);
    static Image createTextureArray(const std::string& image, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device, uint32_t width, uint32_t height, uint32_t layers);
    static VkSampler createTextureSampler(VkPhysicalDevice physicalDevice, VkDevice device, VkFilter minFilter = VK_FILTER_LINEAR, VkFilter magFilter = VK_FILTER_LINEAR);

    Image();
    Image(VkImage image);
    Image(VkImage image, VmaAllocation allocation);
    Image(VmaAllocator allocator, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t layers = 1);
    VkImageView createTextureView( VkDevice device);
    VkImageView createView(VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device);
    void transitionImageLayout(Commands& commands, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkQueue graphicsQueue, VkDevice device);
    void copyFromBuffer(Buffer& src, Commands& commands, VkQueue graphicsQueue, VkDevice device, uint32_t regionWidth, uint32_t regionHeight, uint32_t fullWidth = 0, uint32_t fullHeight = 0);
    void destroy(VmaAllocator allocator);

private:
    VkImage image;
    VmaAllocation allocation;
    uint32_t layerCount = 1;

    static Buffer loadImage(const std::string& image, VmaAllocator allocator, int32_t& width, int32_t& height);
};