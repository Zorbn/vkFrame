#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

struct Buffer {
public:
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocInfo;
    size_t size;

    void destroy(VmaAllocator& allocator);
    void setData(const void* data);
};
