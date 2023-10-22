#include "cge_engine.hh"
#include "simple_render_system.hh"
#include "cge_game_object.hh"
#include "cge_model.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"
#include "cge_camera.hh"
#include "keyboard_movement_controller.hh"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_PATTERN_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <chrono>
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
        SimpleRenderSystem simple_render_system {this->_device, this->_renderer.get_swap_chain_render_pass()};
        CGE_Camera camera{};
        // camera.set_view_direction(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.set_view_target(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        
        auto viewer_object = CGE_Game_Object::_create_game_object(); // stores camera state
        KeyboardMovementController camera_controller{};
        
        auto current_time = std::chrono::high_resolution_clock::now();

        while (!this->_window._should_close()) {
            glfwPollEvents();

            auto new_time = std::chrono::high_resolution_clock::now();
            float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
            current_time = new_time;

            camera_controller.move_in_plane_xz(_window.get_glfw_window(), frame_time, viewer_object);
            camera.set_view_xyz(viewer_object.transform.translation, viewer_object.transform.rotation);

            float aspect = this->_renderer.get_aspect_ratio();
//            camera.set_orthographic_projection(
//                    -aspect, aspect,
//                    -1, 1,
//                    -1, 1);
            camera.set_perspective_projection(
                glm::radians(50.f),
                aspect,
                0.1F,
                10.F
                );

            if (auto command_buffer = this->_renderer.begin_frame()) {
                this->_renderer.begin_swap_chain_render_pass(command_buffer);
                simple_render_system.render_game_objects(command_buffer, this->_game_objects, camera);
                this->_renderer.end_swap_chain_render_pass(command_buffer);
                this->_renderer.end_frame();
            }
        }
        vkDeviceWaitIdle(this->_device.device());
        //this->_window._open_window();
    }


    void
    CGE_Engine::_load_game_objects() {

        std::shared_ptr<CGE_Model> model = this->createCubeModel(this->_device, {0.0f, 0.0f, 0.0f});

        auto cube = CGE_Game_Object::_create_game_object();
        cube.model = model;
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = {0.5f, 0.5f, 0.5f};

        this->_game_objects.push_back(std::move(cube));

//        std::vector<CGE_Model::Vertex> vertices {
//            {{0.0f, -0.5f}, {1.0F, 0.0F, 0.0F}},
//            {{0.5f,  0.5f}, {0.0F, 1.0F, vertex0vertex.0F}},
//            {{-0.5f, 0.5f}, {0.0F, 0.0F, 1.0F}}
//        };
//
//        auto model = std::make_shared<CGE_Model>(this->_device, vertices);
//        auto triangle = CGE_Game_Object::_create_game_object();
//        triangle.model = model;
//        triangle.color = {.1F, .8F, .1F};
//        triangle.transform2d.translation.x = .2F;
//        triangle.transform2d.scale = {2.f, .5f};
//        triangle.transform2d.rotation = .25F * glm::two_pi<float>();
//
//        _game_objects.push_back(std::move(triangle));
    }
}
