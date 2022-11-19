Project should be structured as:

- Inner ll-layer:
    - An improved version of how the project currently is, use Vulkan directly but keep it nice.

    Section off Vulkan API into:
     * Commands,
     * Buffers,
     * Images,
     * Swapchains, (including depth resources, framebuffers, (swapchain) image views)
     * Pipelines

     * Uniforms with T generic data?

- Out hl-layer
    - High level drawing that doesn't depend on API details (outside of shaders).