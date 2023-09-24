#include "cge_engine.hh"
#include "simple_render_system.hh"
#include "cge_game_object.hh"
#include "cge_model.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_PATTERN_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <array>

namespace cge {

    //
    // CONSTRUCTOR
    //
    CGE_Engine::CGE_Engine() {
        this->_load_game_objects();
    }

    //
    // DESTRUCTOR
    //
    CGE_Engine::~CGE_Engine() {
    }

    //
    // RUN THE ENGINE
    //
    void
    CGE_Engine::_run() {
        while (!this->_window._should_close()) {
            SimpleRenderSystem simple_render_system {this->_device, this->_renderer.get_swap_chain_render_pass()};
            glfwPollEvents();
            if (auto command_buffer = this->_renderer.begin_frame()) {
                this->_renderer.begin_swap_chain_render_pass(command_buffer);
                simple_render_system.render_game_objects(command_buffer, this->_game_objects);
                this->_renderer.end_swap_chain_render_pass(command_buffer);
                this->_renderer.end_frame();
            }
            vkDeviceWaitIdle(this->_device.device());
        }
        //this->_window._open_window();
    }


    void
    CGE_Engine::_load_game_objects() {
        std::vector<CGE_Model::Vertex> vertices {
            {{0.0f, -0.5f}, {1.0F, 0.0F, 0.0F}},
            {{0.5f,  0.5f}, {0.0F, 1.0F, 0.0F}},
            {{-0.5f, 0.5f}, {0.0F, 0.0F, 1.0F}}
        };

        auto model = std::make_shared<CGE_Model>(this->_device, vertices);
        auto triangle = CGE_Game_Object::_create_game_object();
        triangle.model = model;
        triangle.color = {.1F, .8F, .1F};
        triangle.transform2d.translation.x = .2F;
        triangle.transform2d.scale = {2.f, .5f};
        triangle.transform2d.rotation = .25F * glm::two_pi<float>();

        _game_objects.push_back(std::move(triangle));
    }
}
