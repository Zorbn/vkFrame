#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#undef VMA_IMPLEMENTATION

#include "app.hpp"

int main() {
    App app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
