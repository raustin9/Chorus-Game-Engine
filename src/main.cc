#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <string>

#include "cge_engine.hh"

int main() {
    cge::CGE_Engine engine = cge::CGE_Engine();
    engine._run();
    return 0;
}
