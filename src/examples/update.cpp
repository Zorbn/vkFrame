#include "../renderer/renderer.hpp"

/*
 * Update:
 * Make a model that swaps between 2 meshes and has 3 instances.
 */

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

struct InstanceData {
public:
    glm::vec3 pos;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 1;
        bindingDescription.stride = sizeof(InstanceData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.resize(1);

        attributeDescriptions[0].binding = 1;
        attributeDescriptions[0].location = 3;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = 0;

        return attributeDescriptions;
    }
};

struct UniformBufferData {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class App {
private:
    Pipeline pipeline;
    RenderPass renderPass;

    Image textureImage;
    VkImageView textureImageView;
    VkSampler textureSampler;

    UniformBuffer<UniformBufferData> ubo;
    Model<InstanceData> updateTestModel;

    uint32_t frameCount = 0;

public:
    void init(VulkanState& vulkanState, int32_t width, int32_t height, uint32_t maxFramesInFlight) {
        vulkanState.swapchain.create(vulkanState.device, vulkanState.physicalDevice, vulkanState.surface, width, height);

        vulkanState.commands.createPool(vulkanState.physicalDevice, vulkanState.device, vulkanState.surface);
        vulkanState.commands.createBuffers(vulkanState.device, maxFramesInFlight);

        textureImage = Image::createTexture("res/testImg.png", vulkanState.allocator, vulkanState.commands, vulkanState.graphicsQueue, vulkanState.device);
        textureImageView = textureImage.createTextureView(vulkanState.device);
        textureSampler = textureImage.createTextureSampler(vulkanState.physicalDevice, vulkanState.device);

        updateTestModel = Model<InstanceData>::fromVerticesAndIndices(testVertices2, testIndices2, 3, vulkanState.allocator, vulkanState.commands, vulkanState.graphicsQueue, vulkanState.device);
        std::vector<InstanceData> instances = {InstanceData{glm::vec3(1.0f, 0.0f, 0.0f)}, InstanceData{glm::vec3(0.0f, 1.0f, 0.0f)}, InstanceData{glm::vec3(0.0f, 0.0f, 1.0f)}};
        updateTestModel.updateInstances(instances, vulkanState.commands, vulkanState.allocator, vulkanState.graphicsQueue, vulkanState.device);

        ubo.create(maxFramesInFlight, vulkanState.allocator);

        renderPass.create(vulkanState.physicalDevice, vulkanState.device, vulkanState.allocator, vulkanState.swapchain, true);

        pipeline.createDescriptorSetLayout(vulkanState.device, [&](std::vector<VkDescriptorSetLayoutBinding>& bindings) {
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
        pipeline.createDescriptorPool(maxFramesInFlight, vulkanState.device);
        pipeline.createDescriptorSets(maxFramesInFlight, vulkanState.device, [&](std::vector<VkWriteDescriptorSet>& descriptorWrites, VkDescriptorSet descriptorSet, size_t i) {
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

            vkUpdateDescriptorSets(vulkanState.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        });
        pipeline.create<InstanceData>("res/updateShader.vert.spv", "res/updateShader.frag.spv", vulkanState.device, renderPass);
    }

    void update(VulkanState& vulkanState) {
        uint32_t animFrame = frameCount / 3000;
        if (frameCount % 3000 == 0) {
            if (animFrame % 2 == 0) {
                updateTestModel.update(testVertices2, testIndices2, vulkanState.commands, vulkanState.allocator, vulkanState.graphicsQueue, vulkanState.device);
            } else {
                updateTestModel.update(testVertices, testIndices, vulkanState.commands, vulkanState.allocator, vulkanState.graphicsQueue, vulkanState.device);
            }
        }

        frameCount++;
    }

    void render(VulkanState& vulkanState, VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame) {
        const VkExtent2D& extent = vulkanState.swapchain.getExtent();

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

        updateTestModel.draw(commandBuffer);

        renderPass.end(commandBuffer);
    }

    void resize(VulkanState& vulkanState, int32_t width, int32_t height) {
        renderPass.recreate(vulkanState.physicalDevice, vulkanState.device, vulkanState.allocator, vulkanState.swapchain);
    }

    void cleanup(VulkanState& vulkanState) {
        pipeline.cleanup(vulkanState.device);
        renderPass.cleanup(vulkanState.allocator, vulkanState.device);

        ubo.destroy(vulkanState.allocator);

        vkDestroySampler(vulkanState.device, textureSampler, nullptr);
        vkDestroyImageView(vulkanState.device, textureImageView, nullptr);
        textureImage.destroy(vulkanState.allocator);

        updateTestModel.destroy(vulkanState.allocator);
    }

    int run() {
        Renderer renderer;

        std::function<void(VulkanState&, int32_t, int32_t, uint32_t)> initCallback = [&](VulkanState& vulkanState, int32_t width, int32_t height, uint32_t maxFramesInFlight) {
            this->init(vulkanState, width, height, maxFramesInFlight);
        };

        std::function<void(VulkanState&)> updateCallback = [&](VulkanState vulkanState) {
            this->update(vulkanState);
        };

        std::function<void(VulkanState&, VkCommandBuffer, uint32_t, uint32_t)> renderCallback = [&](VulkanState& vulkanState, VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame) {

            this->render(vulkanState, commandBuffer, imageIndex, currentFrame);
        };

        std::function<void(VulkanState&, int32_t, int32_t)> resizeCallback = [&](VulkanState& vulkanState, int32_t width, int32_t height) {
            this->resize(vulkanState, width, height);
        };

        std::function<void(VulkanState&)> cleanupCallback = [&](VulkanState& vulkanState) {
            this->cleanup(vulkanState);
        };

        try {
            renderer.run("Update", 640, 480, initCallback, updateCallback, renderCallback, resizeCallback, cleanupCallback);
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