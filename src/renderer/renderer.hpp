#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <vk_mem_alloc.h>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cinttypes>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <algorithm>
#include <functional>

#include "buffer.hpp"
#include "vertex.hpp"
#include "commands.hpp"
#include "queueFamilyIndices.hpp"
#include "model.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "uniformBuffer.hpp"

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

struct VulkanState {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkQueue graphicsQueue;
    VmaAllocator allocator;
    Swapchain swapchain;
    Commands commands;
};

class Renderer {
public:
    void run(const std::string& windowTitle, const uint32_t windowWidth, const uint32_t windowHeight,
        std::function<void(VulkanState& vulkanState, int32_t width, int32_t height, uint32_t maxFramesInFlight)> initCallback,
        std::function<void(VulkanState& vulkanState, VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame)> renderCallback,
        std::function<void(VulkanState& vulkanState)> updateCallback,
        std::function<void(VulkanState& vulkanState, int32_t width, int32_t height)> resizeCallback,
        std::function<void(VulkanState& vulkanState)> cleanupCallback);

private:
    GLFWwindow* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkQueue presentQueue;

    VulkanState vulkanState;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;

    void initWindow(const std::string& windowTitle, const uint32_t windowWidth, const uint32_t windowHeight);

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    void initVulkan(std::function<void(VulkanState& vulkanState, int32_t width, int32_t height, uint32_t maxFramesInFlight)> initCallback);
    void createInstance();
    void createAllocator();
    void createLogicalDevice();

    void mainLoop(std::function<void(VulkanState& vulkanState, VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame)> renderCallback,
        std::function<void(VulkanState& vulkanState)> updateCallback,
        std::function<void(VulkanState& vulkanState, int32_t width, int32_t height)> resizeCallback);
    void drawFrame(std::function<void(VulkanState& vulkanState, VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame)> renderCallback,
        std::function<void(VulkanState& vulkanState, int32_t width, int32_t height)> resizeCallback);
    void waitWhileMinimized();

    void cleanup(std::function<void(VulkanState& vulkanState)> cleanupCallback);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();

    void createSurface();

    void pickPhysicalDevice();

    bool hasStencilComponent(VkFormat format);

    void createUniformBuffers();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createSyncObjects();

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};