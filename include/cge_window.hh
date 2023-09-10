#pragma once
#ifndef WINDOW
#define WINDOW

#include <GLFW/glfw3.h>
#include <string>
#include <cstdint>

namespace cge {
    class CGE_Window {
        public:
            CGE_Window(uint32_t width, uint32_t height, std::string name);
            CGE_Window(CGE_Window&) = delete;
            CGE_Window &operator=(const CGE_Window) = delete;
            ~CGE_Window();
            void _open_window();

        private:
            void _init_window();

            const uint32_t _width;
            const uint32_t _height;
            std::string _window_name;
            GLFWwindow *_window;
    };
} /* end cge namespace */

#endif /* WINDOW */
