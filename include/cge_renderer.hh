#pragma once
#ifndef CGE_RENDERER
#define CGE_RENDERER

#include <memory>
#include <vector>
#include <cassert>
#include <cstdint>

#include <vulkan/vulkan_core.h>
#include "cge_model.hh"
#include "cge_device.hh"
#include "cge_window.hh"
#include "cge_swap_chain.hh"

namespace cge {

    class CGE_Renderer {
        public:
            CGE_Renderer(CGE_Window& window, CGE_Device &device);
            ~CGE_Renderer();

            CGE_Renderer(const CGE_Renderer&) = delete;
            CGE_Renderer& operator=(const CGE_Renderer&) = delete;

            VkCommandBuffer begin_frame();
            void end_frame();

            void begin_swap_chain_render_pass(VkCommandBuffer command_buffer);
            void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

            bool is_frame_started() const { return this->_is_frame_started; }
            VkCommandBuffer get_current_command_buffer() const { 
                assert(this->_is_frame_started && "Cannot get command buffer when frame is not in progress");
                return this->_command_buffers[this->_current_frame_index]; 
            }
            VkRenderPass get_swap_chain_render_pass() const { return this->_swap_chain->getRenderPass(); }

            int get_current_frame_index() const { 
                assert (this->_is_frame_started && "Cannot get frame index when frame is not in progress");
                return this->_current_frame_index; 
            }

        private:
            void _create_command_buffers();
            void _free_command_buffers();
            void _recreate_swap_chain();

            CGE_Window &_window;
            CGE_Device &_device;
            std::unique_ptr<CGE_SwapChain> _swap_chain;
            std::vector<VkCommandBuffer> _command_buffers;
            uint32_t _current_image_index;
            int _current_frame_index{0};
            bool _is_frame_started = false;
    };
}

#endif /* CGE_RENDERER */
