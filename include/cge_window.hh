#pragma once
#include <vulkan/vulkan_core.h>
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
            CGE_Window& operator=(const CGE_Window&) = delete;
            ~CGE_Window();
            void _open_window();
            bool _should_close();
            void _create_window_surface(VkInstance instance, VkSurfaceKHR *surface);
            void _reset_framebuffer_resized() { this->_frame_buffer_resized = false; }
            bool _was_window_resized() { return this->_frame_buffer_resized; }
            VkExtent2D _get_extent(); 

        private:
            static void _frame_buffer_resize_callback(GLFWwindow *window, int width, int height);
            void _init_window();

            int _width;
            int _height;
            bool _frame_buffer_resized = false;
            std::string _window_name;
            GLFWwindow *_window;
    };
} /* end cge namespace */

#endif /* WINDOW */
