include(FetchContent)
include(CheckCCompilerFlag)

cmake_minimum_required(VERSION 3.17)

project(vkFrame)
set(LIB_NAME vkFrame)

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

FetchContent_MakeAvailable(glfw glm vk_mem_alloc)

add_library(
        ${LIB_NAME} STATIC
        src/vkFrame/renderer.cpp src/vkFrame/renderer.hpp
        src/vkFrame/buffer.cpp src/vkFrame/buffer.hpp
        src/vkFrame/commands.cpp src/vkFrame/commands.hpp
        src/vkFrame/swapchain.cpp src/vkFrame/swapchain.hpp
        src/vkFrame/image.cpp src/vkFrame/image.hpp
        src/vkFrame/pipeline.cpp src/vkFrame/pipeline.hpp
        src/vkFrame/renderPass.cpp src/vkFrame/renderPass.hpp
        src/vkFrame/uniformBuffer.hpp
        src/vkFrame/model.hpp
        src/vkFrame/queueFamilyIndices.hpp
        src/vkFrame/headerImpls.cpp
)

target_include_directories(${LIB_NAME} INTERFACE src/vkFrame/..)

check_c_compiler_flag(-Wnullability-completeness HAVE_NULL_COMPLETENESS)

if (HAVE_NULL_COMPLETENESS)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-nullability-completeness)
endif (HAVE_NULL_COMPLETENESS)

target_link_libraries(
        ${LIB_NAME}
        glm
        glfw
        Vulkan::Vulkan
        VulkanMemoryAllocator
)

# Examples

set(ExampleNames UpdateExample CubesExample RenderTextureExample)

add_executable(UpdateExample src/examples/update.cpp)
target_link_libraries(UpdateExample ${LIB_NAME})

add_executable(CubesExample src/examples/cubes.cpp)
target_link_libraries(CubesExample ${LIB_NAME})

add_executable(RenderTextureExample src/examples/renderTexture.cpp)
target_link_libraries(RenderTextureExample ${LIB_NAME})

foreach(EXAMPLE IN LISTS ExampleNames)
        add_custom_command(
                TARGET ${EXAMPLE}
                POST_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
                DEPENDS ${CMAKE_CURRENT_BINARY_DIR}
        )
endforeach()