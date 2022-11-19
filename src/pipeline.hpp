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
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    void create(const std::string& vertShader, const std::string& fragShader, bool enableDepth, Swapchain& swapchain, VkPhysicalDevice& physicalDevice, VkDevice& device);
    void createDescriptorSetLayout(VkDevice device, std::function<void(std::vector<VkDescriptorSetLayoutBinding>&)> setupBindings);
    void createDescriptorPool(const uint32_t maxFramesInFlight, VkDevice device);
    void createDescriptorSets(const uint32_t maxFramesInFlight, VkDevice device, std::function<void(std::vector<VkWriteDescriptorSet>&, std::vector<VkDescriptorSet>&, size_t)> setupDescriptor);
    void cleanup(VkDevice device);

private:
    void createPipeline(const std::string& vertShader, const std::string& fragShader, VkDevice device);
    void createRenderPass(Swapchain& swapchain, VkPhysicalDevice physicalDevice, VkDevice device, bool enableDepth);

    static VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);
    static std::vector<char> readFile(const std::string& filename);
};