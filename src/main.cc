#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <string>
#include <stdexcept>

#include "cge_engine.hh"

int main() {
    cge::CGE_Engine engine = cge::CGE_Engine();

    try {
        engine._run();
    } catch (const std::exception &e) {
        printf("this err\n");
        fprintf(stderr, "Error: %s\n", e.what());
    }
    return 0;
}
