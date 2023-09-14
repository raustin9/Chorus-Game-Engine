#include "cge_engine.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <memory>
#include <stdexcept>

namespace cge {
    //
    // CONSTRUCTOR
    //
    CGE_Engine::CGE_Engine() {
        this->_create_pipeline_layout();
        this->_create_pipeline();
        this->_create_command_buffers();
    }

    //
    // DESTRUCTOR
    //
    CGE_Engine::~CGE_Engine() {
        vkDestroyPipelineLayout(this->_device.device(), this->_pipeline_layout, nullptr);
    }

    //
    // RUN THE ENGINE
    //
    void
    CGE_Engine::_run() {
        this->_window._open_window();
    }

    //
    // Create the pipeline layout info
    //
    void
    CGE_Engine::_create_pipeline_layout() {
        VkPipelineLayoutCreateInfo pipeline_layout_info {};

        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0;
        pipeline_layout_info.pSetLayouts = nullptr;
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(
                this->_device.device(), 
                &pipeline_layout_info, 
                nullptr, 
                &this->_pipeline_layout
            ) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to create pipeline layout info");
        }
    }

    //
    // Create the pipeline
    //
    void 
    CGE_Engine::_create_pipeline() {
        auto pipeline_config = CGE_Pipeline::_default_pipeline_config_info(this->_swap_chain.width(), this->_swap_chain.height());
        pipeline_config._render_pass = this->_swap_chain.getRenderPass();
        pipeline_config._pipeline_layout = this->_pipeline_layout;
        this->_pipeline = std::make_unique<CGE_Pipeline>(
                this->_device,
                "shaders/simple.vert.spv",
                "shaders/simple.frag.spv",
                pipeline_config
            );
    }

    void 
    CGE_Engine::_create_command_buffers() {

    }

    void 
    CGE_Engine::_draw_frame() {

    }
}
