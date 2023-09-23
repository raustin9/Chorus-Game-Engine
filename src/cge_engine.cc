#include "cge_game_object.hh"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_PATTERN_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "cge_engine.hh"
#include "cge_model.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <array>

namespace cge {
    struct SimplePushConstantData {
        glm::mat2 transform{1.F};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    //
    // CONSTRUCTOR
    //
    CGE_Engine::CGE_Engine() {
        this->_load_game_objects();
        this->_create_pipeline_layout();
        this->_recreate_swap_chain();
        this->_create_command_buffers();
        // this->_create_pipeline();
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
            vkDeviceWaitIdle(this->_device.device());
        }
        //this->_window._open_window();
    }

    //
    // Create the pipeline layout info
    //
    void
    CGE_Engine::_create_pipeline_layout() {
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
    CGE_Engine::_recreate_swap_chain() {
        auto extent = this->_window._get_extent();
        while (extent.width == 0 || extent.height == 0) {
            extent = this->_window._get_extent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device.device());
        if (this->_swap_chain == nullptr) {
            this->_swap_chain = std::make_unique<CGE_SwapChain>(this->_device, extent);
        } else {
            this->_swap_chain = std::make_unique<CGE_SwapChain>(this->_device, extent, std::move(this->_swap_chain));
            if (this->_swap_chain->imageCount() != this->_command_buffers.size()) {
                this->_free_command_buffers();
                this->_create_command_buffers();
            }
        }
        this->_create_pipeline();
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

    void
    CGE_Engine::_render_game_objects(VkCommandBuffer command_buffer) {
        this->_pipeline->_bind(command_buffer);

        for (auto& obj: this->_game_objects) {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01F, glm::two_pi<float>());
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

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
    CGE_Engine::_create_pipeline() {
        assert(this->_swap_chain != nullptr && "Cannot create pipeline before swap chain");
        assert(this->_pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");
        // auto pipeline_config = CGE_Pipeline::_default_pipeline_config_info(this->_swap_chain->width(), this->_swap_chain->height());
        PipelineConfigInfo pipeline_config{};
        CGE_Pipeline::_default_pipeline_config_info(pipeline_config);
        pipeline_config._render_pass = this->_swap_chain->getRenderPass();
        pipeline_config._pipeline_layout = this->_pipeline_layout;
        this->_pipeline = std::make_unique<CGE_Pipeline>(
                this->_device,
                "shaders/simple.vert.spv",
                "shaders/simple.frag.spv",
                pipeline_config
            );
    }

    void
    CGE_Engine::_record_command_buffer(int image_index) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(this->_command_buffers[image_index], &begin_info) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = this->_swap_chain->getRenderPass();
        render_pass_info.framebuffer = this->_swap_chain->getFrameBuffer(image_index);

        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = this->_swap_chain->getSwapChainExtent();

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {0.01F, 0.01F, 0.01F, 1.0F};
        clear_values[1].depthStencil = {1.0F, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(this->_command_buffers[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(this->_swap_chain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(this->_swap_chain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, this->_swap_chain->getSwapChainExtent()};
        vkCmdSetViewport(this->_command_buffers[image_index], 0, 1, &viewport);
        vkCmdSetScissor(this->_command_buffers[image_index], 0, 1, &scissor);

        this->_render_game_objects(_command_buffers[image_index]);

//        this->_pipeline->_bind(this->_command_buffers[image_index]);
//        this->_model->_bind(this->_command_buffers[image_index]);
//
//        for (int j = 0; j < 4; j++) {
//            SimplePushConstantData push{};
//            push.offset = {-0.5F + frame * 0.01F, -0.4F + j * 0.25F};
//            push.color = {0.0F, 0.0F, 0.2F + 0.2F * j};
//
//            vkCmdPushConstants(
//                this->_command_buffers[image_index], 
//                this->_pipeline_layout, 
//                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
//                0, 
//                sizeof(SimplePushConstantData), 
//                &push
//            );
//            this->_model->_draw(this->_command_buffers[image_index]);
//        }

        // vkCmdDraw(this->_command_buffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(this->_command_buffers[image_index]);
        if (vkEndCommandBuffer(this->_command_buffers[image_index]) != VK_SUCCESS) {
                throw std::runtime_error("Error: unable to end command buffer");
        }
        
    }

    //
    // Create the buffer of commands to be executed
    //
    void 
    CGE_Engine::_create_command_buffers() {
        this->_command_buffers.resize(this->_swap_chain->imageCount());
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = this->_device.getCommandPool();
        alloc_info.commandBufferCount = static_cast<uint32_t>(this->_command_buffers.size());
        
        if (vkAllocateCommandBuffers(this->_device.device(), &alloc_info, this->_command_buffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to allocate command buffers");
        }

    }

    void 
    CGE_Engine::_free_command_buffers() {
        vkFreeCommandBuffers(
                this->_device.device()
                , this->_device.getCommandPool()
                , static_cast<uint32_t>(this->_command_buffers.size())
                , _command_buffers.data());

        this->_command_buffers.clear();
    }

    void 
    CGE_Engine::_draw_frame() {
        uint32_t image_index;
        auto result = this->_swap_chain->acquireNextImage(&image_index);
        // std::cout << "Image index " << image_index << std::endl;

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            this->_recreate_swap_chain();
            return;
        }
        
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Error: failed to acquire swap chain image");
        }

        this->_record_command_buffer(image_index);

        result = this->_swap_chain->submitCommandBuffers(&this->_command_buffers[image_index], &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR
            || result == VK_SUBOPTIMAL_KHR
            || this->_window._was_window_resized()) {
            this->_window._reset_framebuffer_resized();
            this->_recreate_swap_chain();
            return;
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to present swap chain image");
        }

    }
}
