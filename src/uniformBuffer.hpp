#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "buffer.hpp"

template <typename T>
struct UniformBuffer {
public:
    void create(const size_t maxFramesInFlight, VmaAllocator allocator) {
        VkDeviceSize bufferByteSize = sizeof(T);

        buffers.resize(maxFramesInFlight);
        buffersMapped.resize(maxFramesInFlight);

        for (size_t i = 0; i < maxFramesInFlight; i++) {
            buffers[i] = Buffer::create(allocator, bufferByteSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true);

            vmaMapMemory(allocator, buffers[i].allocation, &buffersMapped[i]);
        }
    }

    void update(const T& data) {
        size_t bufferCount = buffersMapped.size();
        for (size_t i = 0; i < bufferCount; i++) {
            memcpy(buffersMapped[i], &data, sizeof(T));
        }
    }

    VkBuffer getBuffer(size_t i) {
        return buffers[i].buffer;
    }

    size_t getDataSize() {
        return sizeof(T);
    }

    void destroy(VmaAllocator allocator) {
        size_t bufferCount = buffers.size();
        for (size_t i = 0; i < bufferCount; i++) {
            vmaUnmapMemory(allocator, buffers[i].allocation);
            buffers[i].destroy(allocator);
        }
    }

private:
    std::vector<Buffer> buffers;
    std::vector<void*> buffersMapped;
};