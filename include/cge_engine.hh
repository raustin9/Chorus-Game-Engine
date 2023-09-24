#pragma once
#ifndef ENGINE
#define ENGINE

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "cge_model.hh"
#include "cge_device.hh"
#include "cge_window.hh"
#include "cge_renderer.hh"
#include "cge_game_object.hh"

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
            void _load_game_objects();

            CGE_Window _window = CGE_Window(WIDTH, HEIGHT, "Chorus Engine");
            CGE_Device _device {_window};
            CGE_Renderer _renderer {this->_window, this->_device};
            std::unique_ptr<CGE_Model> _model;
            std::vector<CGE_Game_Object> _game_objects;
    };
}

#endif /* ENGINE */
