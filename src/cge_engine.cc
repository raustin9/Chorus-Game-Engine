#include "cge_engine.hh"
#include "simple_render_system.hh"
#include "cge_game_object.hh"
#include "cge_model.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"
#include "cge_camera.hh"
#include "cge_buffer.hh"
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
#include <numeric>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <array>

namespace cge {

    struct Global_Ubo {
        alignas(16) glm::mat4 projectionView{1.f};
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
    };

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
        auto min_offset_alignment = std::lcm(
            _device.properties.limits.minUniformBufferOffsetAlignment,
            _device.properties.limits.nonCoherentAtomSize);

        CGE_Buffer global_ubo {
            _device,
            sizeof(Global_Ubo),
            CGE_SwapChain::MAX_FRAMES_IN_FLIGHT,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            min_offset_alignment
        };
        global_ubo.map();

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
            camera.set_perspective_projection(
                glm::radians(50.f),
                aspect,
                0.1F,
                10.F
                );

            if (auto command_buffer = this->_renderer.begin_frame()) {
                int frame_index = _renderer.get_current_frame_index();
                FrameInfo frame_info {
                    frame_index,
                    frame_time,
                    command_buffer,
                    camera
                };

                // Update
                Global_Ubo ubo{};
                ubo.projectionView = camera.get_projection_matrix() * camera.get_view_matrix();
                global_ubo.write_to_index(&ubo, frame_index);
                global_ubo.flush_index(frame_index);

                // Render
                this->_renderer.begin_swap_chain_render_pass(command_buffer);
                simple_render_system.render_game_objects(frame_info, this->_game_objects);
                this->_renderer.end_swap_chain_render_pass(command_buffer);
                this->_renderer.end_frame();
            }
        }
        vkDeviceWaitIdle(this->_device.device());
    }


    void
    CGE_Engine::_load_game_objects() {

        std::shared_ptr<CGE_Model> model 
            = CGE_Model::create_model_from_file(_device, "models/colored_cube.obj");

        auto game_object = CGE_Game_Object::_create_game_object();
        game_object.model = model;
        game_object.transform.translation = {0.0f, 0.0f, 2.5f};
        // game_object.transform.scale = {0.5f, 0.5f, 0.5f};
        game_object.transform.scale = glm::vec3(0.5f); 

        this->_game_objects.push_back(std::move(game_object));
    }
}
