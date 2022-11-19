#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <fstream>

#include "vertex.hpp"
#include "customInstanceData.hpp"
#include "swapchain.hpp"

class Pipeline {
public:
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    void create(const std::string& vertShader, const std::string& fragShader, Swapchain& swapchain, VkPhysicalDevice& physicalDevice, VkDevice& device);
    void cleanup(VkDevice device);

private:
    void createPipeline(const std::string& vertShader, const std::string& fragShader, VkDevice device);
    void createRenderPass(Swapchain& swapchain, VkPhysicalDevice physicalDevice, VkDevice device);
    void createDescriptorSetLayout(VkDevice device);
    VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);

    static std::vector<char> readFile(const std::string& filename);
};