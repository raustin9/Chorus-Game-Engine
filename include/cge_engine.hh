#pragma once
#ifndef ENGINE
#define ENGINE

#include "cge_window.hh"
#include "cge_pipeline.hh"
#include "cge_device.hh"

namespace cge {
    class CGE_Engine {
        public:
            CGE_Engine();
            ~CGE_Engine();
            
            void _run();
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

        private:
            CGE_Window _window = CGE_Window(WIDTH, HEIGHT, "Chorus Engine");
            CGE_Device _device {_window};
            CGE_Pipeline _pipeline = CGE_Pipeline(_device, "shaders/simple.vert.spv", "shaders/simple.frag.spv", CGE_Pipeline::_default_pipeline_config_info(WIDTH, HEIGHT));
    };
}

#endif /* ENGINE */
