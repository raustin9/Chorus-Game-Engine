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

            std::unique_ptr<CGE_Model> createCubeModel(CGE_Device& device, glm::vec3 offset) {
                std::vector<CGE_Model::Vertex> vertices{

                    // left face (white)
                    {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
                        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
                        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
                        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
                        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
                        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

                        // right face (yellow)
                        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
                        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
                        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
                        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
                        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
                        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

                        // top face (orange, remember y axis points down)
                        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
                        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
                        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

                        // bottom face (red)
                        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
                        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
                        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
                        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

                        // nose face (blue)
                        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
                        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
                        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
                        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
                        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
                        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

                        // tail face (green)
                        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
                        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
                        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

                };
                for (auto& v : vertices) {
                    v.position += offset;
                }
                return std::make_unique<CGE_Model>(device, vertices);
            }

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
