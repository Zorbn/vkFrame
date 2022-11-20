#include "image.hpp"

Image::Image() {}

Image::Image(VkImage image) : image(image) {}

Image::Image(VkImage image, VmaAllocation allocation) : image(image), allocation(allocation) {}

Image::Image(VmaAllocator allocator, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t layers) {
    layerCount = layers;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = layerCount;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo aci = {};
    aci.usage = VMA_MEMORY_USAGE_AUTO;

    VkImage image;
    VmaAllocation allocation;

    if (vmaCreateImage(allocator, &imageInfo, &aci, &image, &allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate image memory!");
    }

    this->image = image;
    this->allocation = allocation;
}

Buffer Image::loadImage(const std::string& image, VmaAllocator allocator, int32_t& width, int32_t& height) {
    int32_t texChannels;
    stbi_uc* pixels = stbi_load(image.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
    size_t imageSize = width * height;
    VkDeviceSize imageByteSize = imageSize * 4;

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image!");
    }

    Buffer stagingBuffer(allocator, imageByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    stagingBuffer.setData(pixels);

    stbi_image_free(pixels);

    return stagingBuffer;
}

Image Image::createTexture(const std::string& image, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device) {
    int32_t texWidth, texHeight;
    Buffer stagingBuffer = loadImage(image, allocator, texWidth, texHeight);

    Image textureImage = Image(allocator, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    textureImage.transitionImageLayout(commands, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, graphicsQueue, device);
    textureImage.copyFromBuffer(stagingBuffer, commands, graphicsQueue, device, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    textureImage.transitionImageLayout(commands, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, graphicsQueue, device);

    stagingBuffer.destroy(allocator);

    return textureImage;
}

Image Image::createTextureArray(const std::string& image, VmaAllocator allocator, Commands& commands, VkQueue graphicsQueue, VkDevice device, uint32_t width, uint32_t height, uint32_t layers) {
    int32_t texWidth, texHeight;
    Buffer stagingBuffer = loadImage(image, allocator, texWidth, texHeight);

    Image textureImage = Image(allocator, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layers);

    textureImage.transitionImageLayout(commands, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, graphicsQueue, device);
    textureImage.copyFromBuffer(stagingBuffer, commands, graphicsQueue, device, width, height, texWidth, texHeight);
    textureImage.transitionImageLayout(commands, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, graphicsQueue, device);

    stagingBuffer.destroy(allocator);

    return textureImage;
}

VkImageView Image::createTextureView(VkDevice device) {
    return createView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, device);
}

VkSampler Image::createTextureSampler(VkPhysicalDevice physicalDevice, VkDevice device, VkFilter minFilter, VkFilter magFilter) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkSampler textureSampler;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler!");
    }

    return textureSampler;
}

VkImageView Image::createView(VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = layerCount == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture image view!");
    }

    return imageView;
}

void Image::transitionImageLayout(Commands& commands, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkQueue graphicsQueue, VkDevice device) {
    VkCommandBuffer commandBuffer = commands.beginSingleTime(graphicsQueue, device);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    commands.endSingleTime(commandBuffer, graphicsQueue, device);
}

void Image::copyFromBuffer(Buffer& src, Commands& commands, VkQueue graphicsQueue, VkDevice device, uint32_t regionWidth, uint32_t regionHeight, uint32_t fullWidth, uint32_t fullHeight) {
    if (fullWidth == 0) {
        fullWidth = regionWidth;
    }

    if (fullHeight == 0) {
        fullHeight = regionHeight;
    }

    VkCommandBuffer commandBuffer = commands.beginSingleTime(graphicsQueue, device);

    std::vector<VkBufferImageCopy> regions;
    uint32_t texPerRow = fullWidth / regionWidth;

    for (uint32_t layer = 0; layer < layerCount; layer++) {
        uint32_t xLayer = layer % texPerRow;
        uint32_t yLayer = layer / texPerRow;

        VkBufferImageCopy region = {};
        region.bufferOffset = (xLayer * regionWidth + yLayer * regionHeight * fullWidth) * 4;
        region.bufferRowLength = fullWidth;
        region.bufferImageHeight = fullHeight;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = layer;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            regionWidth,
            regionHeight,
            1
        };
        regions.push_back(region);
    }

    vkCmdCopyBufferToImage(commandBuffer, src.getBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());

    commands.endSingleTime(commandBuffer, graphicsQueue, device);
}

void Image::destroy(VmaAllocator allocator) {
    vmaDestroyImage(allocator, image, allocation);
}