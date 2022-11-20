#include "renderer.hpp"

const std::vector<Vertex> testVertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> testIndices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

const std::vector<Vertex> testVertices2 = {
    {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> testIndices2 = {
    0, 1, 2
};

class App {
public:
    Pipeline pipeline;
    RenderPass renderPass;

    Image textureImage;
    VkImageView textureImageView;
    VkSampler textureSampler;

    UniformBuffer<UniformBufferData> ubo;
    Model<CustomInstanceData> updateTestModel;

    uint32_t frameCount = 0;

    void init(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkQueue graphicsQueue, VmaAllocator allocator, uint32_t width, uint32_t height,
                uint32_t maxFramesInFlight, Swapchain& swapchain, Commands& commands) {
        swapchain.create(device, physicalDevice, surface, width, height);

        commands.createPool(physicalDevice, device, surface);
        commands.createBuffers(device, maxFramesInFlight);

        textureImage = Image::createTexture("res/testImg.png", allocator, commands, graphicsQueue, device);
        textureImageView = textureImage.createTextureView(device);
        textureSampler = textureImage.createTextureSampler(physicalDevice, device);

        updateTestModel = Model<CustomInstanceData>::fromVerticesAndIndicesModifiable(testVertices2, testIndices2, 8, 12, 4, allocator, commands, graphicsQueue, device);
        ubo.create(maxFramesInFlight, allocator);

        renderPass.create(physicalDevice, device, allocator, swapchain, true);

        pipeline.createDescriptorSetLayout(device, [&](std::vector<VkDescriptorSetLayoutBinding>& bindings) {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            bindings.push_back(uboLayoutBinding);
            bindings.push_back(samplerLayoutBinding);
        });
        pipeline.createDescriptorPool(maxFramesInFlight, device);
        pipeline.createDescriptorSets(maxFramesInFlight, device, [&](std::vector<VkWriteDescriptorSet>& descriptorWrites, VkDescriptorSet descriptorSet, size_t i) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = ubo.getBuffer(i);
            bufferInfo.offset = 0;
            bufferInfo.range = ubo.getDataSize();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImageView;
            imageInfo.sampler = textureSampler;

            descriptorWrites.resize(2);

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSet;
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSet;
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        });
        pipeline.create("res/shader.vert.spv", "res/shader.frag.spv", device, renderPass);
    }

    void render(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VmaAllocator allocator, Swapchain& swapchain, Commands& commands, const uint32_t imageIndex, const uint32_t currentFrame) {
        const VkExtent2D& extent = swapchain.getExtent();

        renderPass.begin(imageIndex, commandBuffer, extent, 0.0f, 0.0f, 0.0f, 1.0f);
        pipeline.bind(commandBuffer, currentFrame);

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferData uboData{};
        uboData.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        uboData.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        uboData.proj = glm::perspective(glm::radians(45.0f), extent.width / (float) extent.height, 0.1f, 10.0f);
        uboData.proj[1][1] *= -1;

        ubo.update(uboData);

        std::vector<CustomInstanceData> instances = {CustomInstanceData{glm::vec3(1.0f, 0.0f, 0.0f)}, CustomInstanceData{glm::vec3(0.0f, 1.0f, 0.0f)}, CustomInstanceData{glm::vec3(0.0f, 0.0f, 1.0f)}};
        updateTestModel.updateInstances(instances, commands, allocator, graphicsQueue, device);
        updateTestModel.draw(commandBuffer);

        renderPass.end(commandBuffer);
    }

    void update(VkDevice device, VkQueue graphicsQueue, VmaAllocator allocator, Commands& commands) {
        uint32_t animFrame = frameCount / 3000;
        if (frameCount % 3000 == 0) {
            if (animFrame % 2 == 0) {
                updateTestModel.update(testVertices2, testIndices2, commands, allocator, graphicsQueue, device);
            } else {
                updateTestModel.update(testVertices, testIndices, commands, allocator, graphicsQueue, device);
            }
        }

        frameCount++;
    }

    void resize(VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator allocator, Swapchain& swapchain, int32_t windowWidth, int32_t windowHeight) {
        renderPass.recreate(physicalDevice, device, allocator, swapchain);
    }

    void cleanup(VkDevice device, VmaAllocator allocator) {
        pipeline.cleanup(device);
        renderPass.cleanup(allocator, device);

        ubo.destroy(allocator);

        vkDestroySampler(device, textureSampler, nullptr);
        vkDestroyImageView(device, textureImageView, nullptr);
        textureImage.destroy(allocator);

        updateTestModel.destroy(allocator);
    }

    int run() {
        Renderer renderer;

        // TODO: Make a struct that gets passed to these types of functions, because this is seriously horrible.
        std::function<void(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkQueue graphicsQueue, VmaAllocator allocator, uint32_t width, uint32_t height,
            uint32_t maxFramesInFlight, Swapchain& swapchain, Commands& commands)> initCallback = [&](VkPhysicalDevice physicalDevice, VkDevice device,
            VkSurfaceKHR surface, VkQueue graphicsQueue, VmaAllocator allocator, uint32_t width, uint32_t height, uint32_t maxFramesInFlight,
            Swapchain& swapchain, Commands& commands) {

            this->init(physicalDevice, device, surface, graphicsQueue, allocator, width, height, maxFramesInFlight, swapchain, commands);
        };

        std::function<void(VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VmaAllocator allocator, Swapchain& swapchain,
            Commands& commands, const uint32_t imageIndex, const uint32_t currentFrame)> renderCallback = [&](VkDevice device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VmaAllocator allocator, Swapchain& swapchain,
            Commands& commands, const uint32_t imageIndex, const uint32_t currentFrame) {

            this->render(device, commandBuffer, graphicsQueue, allocator, swapchain, commands, imageIndex, currentFrame);
        };

        std::function<void(VkDevice device, VkQueue graphicsQueue, VmaAllocator allocator, Commands& commands)> updateCallback = [&](VkDevice device, VkQueue graphicsQueue, VmaAllocator allocator, Commands& commands) {
            this->update(device, graphicsQueue, allocator, commands);
        };

        std::function<void(VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator allocator, Swapchain& swapchain, int32_t windowWidth, int32_t windowHeight)> resizeCallback = [&](VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator allocator, Swapchain& swapchain, int32_t windowWidth, int32_t windowHeight) {
            this->resize(physicalDevice, device, allocator, swapchain, windowWidth, windowHeight);
        };

        std::function<void(VkDevice device, VmaAllocator allocator)> cleanupCallback = [&](VkDevice device, VmaAllocator allocator) {
            this->cleanup(device, allocator);
        };

        try {
            renderer.run("Hello World", 640, 480, initCallback, renderCallback, updateCallback, resizeCallback, cleanupCallback);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
};

int main() {
    App app;
    return app.run();
}