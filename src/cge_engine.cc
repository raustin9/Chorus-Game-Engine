#include "cge_engine.hh"
#include "cge_model.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <memory>
#include <stdexcept>
#include <array>

namespace cge {
    //
    // CONSTRUCTOR
    //
    CGE_Engine::CGE_Engine() {
        this->_load_models();
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
        while (!this->_window._should_close()) {
            glfwPollEvents();
            this->_draw_frame();
        }
        //this->_window._open_window();
        vkDeviceWaitIdle(this->_device.device());
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

    void
    CGE_Engine::_load_models() {
        std::vector<CGE_Model::Vertex> vertices {
            {{0.0f, -0.5f}},
            {{0.5f,  0.5f}},
            {{-0.5f, 0.5f}}
        };

        this->_model = std::make_unique<CGE_Model>(this->_device, vertices);
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

    //
    // Create the buffer of commands to be executed
    //
    void 
    CGE_Engine::_create_command_buffers() {
        this->_command_buffers.resize(this->_swap_chain.imageCount());
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = this->_device.getCommandPool();
        alloc_info.commandBufferCount = static_cast<uint32_t>(this->_command_buffers.size());
        
        if (vkAllocateCommandBuffers(this->_device.device(), &alloc_info, this->_command_buffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to allocate command buffers");
        }

        for (size_t i = 0; i < this->_command_buffers.size(); i++) {
            VkCommandBufferBeginInfo begin_info{};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(this->_command_buffers[i], &begin_info) != VK_SUCCESS) {
                throw std::runtime_error("Error: failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo render_pass_info{};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = this->_swap_chain.getRenderPass();
            render_pass_info.framebuffer = this->_swap_chain.getFrameBuffer(i);

            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = this->_swap_chain.getSwapChainExtent();

            std::array<VkClearValue, 2> clear_values{};
            clear_values[0].color = {0.1F, 0.1F, 0.1F, 1.0F};
            clear_values[1].depthStencil = {1.0F, 0};

            render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
            render_pass_info.pClearValues = clear_values.data();

            vkCmdBeginRenderPass(this->_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

            this->_pipeline->_bind(this->_command_buffers[i]);
            this->_model->_bind(this->_command_buffers[i]);
            this->_model->_draw(this->_command_buffers[i]);
            // vkCmdDraw(this->_command_buffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(this->_command_buffers[i]);
            if (vkEndCommandBuffer(this->_command_buffers[i]) != VK_SUCCESS) {
                    throw std::runtime_error("Error: unable to end command buffer");
            }
        }
    }

    void 
    CGE_Engine::_draw_frame() {
        uint32_t image_index;
        auto result = this->_swap_chain.acquireNextImage(&image_index);
        
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Error: failed to acquire swap chain image");
        }

        result = this->_swap_chain.submitCommandBuffers(&this->_command_buffers[image_index], &image_index);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to present swap chain image");
        }

    }
}
