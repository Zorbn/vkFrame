include(FetchContent)
include(CheckCCompilerFlag)

cmake_minimum_required(VERSION 3.17)

project(vk_cubes)
set(LIB_NAME vk_cubes)

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
        ${LIB_NAME}
        src/renderer/renderer.cpp src/renderer/renderer.hpp
        src/renderer/buffer.cpp src/renderer/buffer.hpp
        src/renderer/commands.cpp src/renderer/commands.hpp
        src/renderer/swapchain.cpp src/renderer/swapchain.hpp
        src/renderer/image.cpp src/renderer/image.hpp
        src/renderer/pipeline.cpp src/renderer/pipeline.hpp
        src/renderer/renderPass.cpp src/renderer/renderPass.hpp
        src/renderer/uniformBuffer.hpp
        src/renderer/model.hpp
        src/renderer/queueFamilyIndices.hpp
        src/renderer/headerImpls.cpp
)

check_c_compiler_flag(-Wnullability-completeness HAVE_NULL_COMPLETENESS)

if (HAVE_NULL_COMPLETENESS)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-nullability-completeness)
endif (HAVE_NULL_COMPLETENESS)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/deps)

target_link_libraries(
        ${LIB_NAME}
        glm
        glfw
        Vulkan::Vulkan
        VulkanMemoryAllocator
)

add_custom_command(
        TARGET ${LIB_NAME}
        POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}
)

# Examples

add_executable(UpdateExample src/examples/update.cpp)
target_link_libraries(UpdateExample ${LIB_NAME})

add_executable(CubesExample src/examples/cubes.cpp)
target_link_libraries(CubesExample ${LIB_NAME})

add_executable(SsaoExample src/examples/ssao.cpp)
target_link_libraries(SsaoExample ${LIB_NAME})

add_executable(DeferredExample src/examples/deferred.cpp)
target_link_libraries(DeferredExample ${LIB_NAME})