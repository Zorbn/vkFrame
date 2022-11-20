#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <functional>

#include "vertex.hpp"
#include "customInstanceData.hpp"
#include "swapchain.hpp"

class Pipeline {
public:
    VkRenderPass renderPass;

    void create(const std::string& vertShader, const std::string& fragShader, bool enableDepth, Swapchain& swapchain, VkPhysicalDevice& physicalDevice, VkDevice& device);
    void createDescriptorSetLayout(VkDevice device, std::function<void(std::vector<VkDescriptorSetLayoutBinding>&)> setupBindings);
    void createDescriptorPool(const uint32_t maxFramesInFlight, VkDevice device);
    void createDescriptorSets(const uint32_t maxFramesInFlight, VkDevice device, std::function<void(std::vector<VkWriteDescriptorSet>&, VkDescriptorSet, size_t)> setupDescriptor);
    void cleanup(VkDevice device);

    void beginPass(const uint32_t imageIndex, const uint32_t currentFrame, VkCommandBuffer commandBuffer, Swapchain& swapchain, float clearColorR, float clearColorB, float clearColorG, float clearColorA);
    void endPass(VkCommandBuffer commandBuffer);

private:
    void createPipeline(const std::string& vertShader, const std::string& fragShader, VkDevice device);
    void createRenderPass(Swapchain& swapchain, VkPhysicalDevice physicalDevice, VkDevice device, bool enableDepth);
    void bind(VkCommandBuffer commandBuffer, int32_t currentFrame);

    static VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);
    static std::vector<char> readFile(const std::string& filename);

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
};