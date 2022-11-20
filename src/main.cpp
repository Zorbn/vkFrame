#include "renderer.hpp"

int main() {
    Renderer renderer;

    try {
        renderer.run("Hello World", 640, 480);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
