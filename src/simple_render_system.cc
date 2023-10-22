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
    struct SimplePushConstantData {
        glm::mat4 transform{1.F};
        // glm::vec2 offset;
        // alignas(16) glm::vec3 color;
        glm::mat4 normalMatrix{1.f};
    };

    //
    // CONSTRUCTOR
    //
    SimpleRenderSystem::SimpleRenderSystem(CGE_Device &device, VkRenderPass render_pass) : _device{device} {
        this->_create_pipeline_layout();
        this->_create_pipeline(render_pass);
    }

    //
    // DESTRUCTOR
    //
    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(this->_device.device(), this->_pipeline_layout, nullptr);
    }

    //
    // Create the pipeline layout info
    //
    void
    SimpleRenderSystem::_create_pipeline_layout() {
        VkPushConstantRange push_constant_range{};
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset = 0;
        push_constant_range.size = sizeof(SimplePushConstantData);
        

        VkPipelineLayoutCreateInfo pipeline_layout_info {};

        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0;
        pipeline_layout_info.pSetLayouts = nullptr;
        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pPushConstantRanges = &push_constant_range;

        if (vkCreatePipelineLayout(
                this->_device.device(), 
                &pipeline_layout_info, 
                nullptr, 
                &this->_pipeline_layout
            ) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to create pipeline layout info");
        }
    }

    void
    SimpleRenderSystem::render_game_objects(
            VkCommandBuffer command_buffer, 
            std::vector<CGE_Game_Object>& game_objects,
            const CGE_Camera& camera) {
        this->_pipeline->_bind(command_buffer);

        auto projection_view = camera.get_projection_matrix() * camera.get_view_matrix();

        for (auto& obj: game_objects) {
            SimplePushConstantData push{};
            auto model_matrix = obj.transform.mat4();
            push.transform = projection_view * model_matrix;
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                command_buffer, 
                this->_pipeline_layout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                0, 
                sizeof(SimplePushConstantData), 
                &push
            );
            obj.model->_bind(command_buffer);
            obj.model->_draw(command_buffer);
        }
    }

    //
    // Create the pipeline
    //
    void 
    SimpleRenderSystem::_create_pipeline(VkRenderPass render_pass) {
        assert(this->_pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipeline_config{};
        CGE_Pipeline::_default_pipeline_config_info(pipeline_config);
        pipeline_config._render_pass = render_pass;
        pipeline_config._pipeline_layout = this->_pipeline_layout;
        this->_pipeline = std::make_unique<CGE_Pipeline>(
                this->_device,
                "shaders/vert/simple.vert.spv",
                "shaders/frag/simple.frag.spv",
                pipeline_config
            );
    }
}
