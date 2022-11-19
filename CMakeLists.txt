include(FetchContent)
include(CheckCCompilerFlag)

cmake_minimum_required(VERSION 3.17)

project(gFps)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
)
FetchContent_Declare(
        vk_mem_alloc
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG v3.0.1
)
FetchContent_Declare(
        vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
        GIT_TAG v0.11
)

FetchContent_MakeAvailable(glfw glm vk_mem_alloc vk_bootstrap)
add_subdirectory(deps/glad)

add_executable(
        game
        deps/stb_image.h
        src/main.cpp
        src/app.cpp src/app.hpp
        src/buffer.cpp src/buffer.hpp
        src/commands.cpp src/commands.hpp
        src/vertex.cpp src/vertex.hpp
        src/instanceData.cpp src/instanceData.hpp
        src/swapchain.cpp src/swapchain.hpp
        src/image.cpp src/image.hpp
        src/pipeline.cpp src/pipeline.hpp
        src/model.hpp
        src/queueFamilyIndices.hpp
        src/customInstanceData.hpp
)

check_c_compiler_flag(-Wnullability-completeness HAVE_NULL_COMPLETENESS)

if (HAVE_NULL_COMPLETENESS)
        target_compile_options(game PRIVATE -Wno-nullability-completeness)
endif (HAVE_NULL_COMPLETENESS)

target_link_libraries(
        game
        glm
        glfw
        glad
        vk-bootstrap::vk-bootstrap
        Vulkan::Vulkan
        VulkanMemoryAllocator
)

add_custom_command(
        TARGET game
        POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}
)
