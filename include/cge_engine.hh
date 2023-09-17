#pragma once
#ifndef ENGINE
#define ENGINE

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "cge_model.hh"
#include "cge_device.hh"
#include "cge_window.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"

namespace cge {
    class CGE_Engine {
        public:
            CGE_Engine();
            ~CGE_Engine();

            CGE_Engine(const CGE_Engine&) = delete;
            CGE_Engine& operator=(const CGE_Engine&) = delete;
            
            void _run();
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

        private:
            void _load_models();
            void _create_pipeline_layout();
            void _create_pipeline();
            void _create_command_buffers();
            void _free_command_buffers();
            void _draw_frame();
            void _recreate_swap_chain();
            void _record_command_buffer(int image_index);

            CGE_Window _window = CGE_Window(WIDTH, HEIGHT, "Chorus Engine");
            CGE_Device _device {_window};
            std::unique_ptr<CGE_SwapChain> _swap_chain;
            // CGE_SwapChain _swap_chain {this->_device, this->_window._get_extent()};
            VkPipelineLayout _pipeline_layout;
            std::unique_ptr<CGE_Pipeline> _pipeline;
            std::vector<VkCommandBuffer> _command_buffers;
            std::unique_ptr<CGE_Model> _model;

            // CGE_Pipeline _pipeline = CGE_Pipeline(_device, "shaders/simple.vert.spv", "shaders/simple.frag.spv", CGE_Pipeline::_default_pipeline_config_info(WIDTH, HEIGHT));
    };
}

#endif /* ENGINE */
