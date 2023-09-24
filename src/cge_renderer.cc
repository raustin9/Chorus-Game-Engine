#include "cge_renderer.hh"
#include "cge_model.hh"
#include "cge_game_object.hh"
#include "cge_pipeline.hh"
#include "cge_swap_chain.hh"

#include <cstdint>
#include <iostream>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <array>

namespace cge {
    //
    // CONSTRUCTOR
    //
    CGE_Renderer::CGE_Renderer(CGE_Window &window, CGE_Device &device) : _window{window}, _device{device} {
        this->_recreate_swap_chain();
        this->_create_command_buffers();
    }

    //
    // DESTRUCTOR
    //
    CGE_Renderer::~CGE_Renderer() {
        this->_free_command_buffers();
    }

    void
    CGE_Renderer::_recreate_swap_chain() {
        auto extent = this->_window._get_extent();
        while (extent.width == 0 || extent.height == 0) {
            extent = this->_window._get_extent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device.device());
        if (this->_swap_chain == nullptr) {
            this->_swap_chain = std::make_unique<CGE_SwapChain>(this->_device, extent);
        } else {
            std::shared_ptr<CGE_SwapChain> old_swap_chain = std::move(this->_swap_chain);
            this->_swap_chain = std::make_unique<CGE_SwapChain>(this->_device, extent, old_swap_chain);

            if (!old_swap_chain->compareSwapFormats(*this->_swap_chain.get())) {
                throw new std::runtime_error("Swap chain image or depth format has changed");
            }
        }
    }


    //
    // Create the buffer of commands to be executed
    //
    void 
    CGE_Renderer::_create_command_buffers() {
        this->_command_buffers.resize(CGE_SwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = this->_device.getCommandPool();
        alloc_info.commandBufferCount = static_cast<uint32_t>(this->_command_buffers.size());
        
        if (vkAllocateCommandBuffers(this->_device.device(), &alloc_info, this->_command_buffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to allocate command buffers");
        }

    }

    //
    // Free the commad buffers
    //
    void 
    CGE_Renderer::_free_command_buffers() {
        vkFreeCommandBuffers(
                this->_device.device()
                , this->_device.getCommandPool()
                , static_cast<uint32_t>(this->_command_buffers.size())
                , _command_buffers.data());

        this->_command_buffers.clear();
    }

    //
    // Begin logic for the beginning drawing a frame
    //
    VkCommandBuffer
    CGE_Renderer::begin_frame() {
        assert(!this->_is_frame_started && "Cannot call begin_frame() while frame is already in progress");
        auto result = this->_swap_chain->acquireNextImage(&this->_current_image_index);
        // std::cout << "Image index " << image_index << std::endl;

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            this->_recreate_swap_chain();
            return nullptr;
        }
        
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Error: failed to acquire swap chain image");
        }

        this->_is_frame_started = true;

        auto command_buffer = this->get_current_command_buffer();
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to begin recording command buffer");
        }

        return command_buffer;
    }

    //
    // Logic for the end of drawing a frame
    //
    void
    CGE_Renderer::end_frame() {
        assert(this->_is_frame_started && "Cannot call end_frame() while frame is not in progress");
        auto command_buffer = this->get_current_command_buffer();
        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Error: unable to end command buffer");
        }
        auto result = this->_swap_chain->submitCommandBuffers(&command_buffer, &this->_current_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR
            || result == VK_SUBOPTIMAL_KHR
            || this->_window._was_window_resized()) {
            this->_window._reset_framebuffer_resized();
            this->_recreate_swap_chain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to present swap chain image");
        }

        this->_is_frame_started = false;
        this->_current_frame_index = (this->_current_frame_index + 1) % CGE_SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    //
    // Logic for beginning of swap chain render pass
    //
    void
    CGE_Renderer::begin_swap_chain_render_pass(VkCommandBuffer command_buffer) {
        assert(this->_is_frame_started && "Cannot call begin_swap_chain_render_pass() while frame is not in progress");
        assert(command_buffer == this->get_current_command_buffer() && "Cannot begin render pass for command buffer from a different frame");

        // 11:59 in tutorial vid 11 -- CONTINUE
        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = this->_swap_chain->getRenderPass();
        render_pass_info.framebuffer = this->_swap_chain->getFrameBuffer(this->_current_image_index);

        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = this->_swap_chain->getSwapChainExtent();

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {0.01F, 0.01F, 0.01F, 1.0F};
        clear_values[1].depthStencil = {1.0F, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(this->_swap_chain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(this->_swap_chain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, this->_swap_chain->getSwapChainExtent()};
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    }

    //
    // Logic for end of swap chain render pass
    //
    void
    CGE_Renderer::end_swap_chain_render_pass(VkCommandBuffer command_buffer) {
        assert(this->_is_frame_started && "Cannot call end_swap_chain_render_pass() while frame is not in progress");
        assert(command_buffer == this->get_current_command_buffer() && "Cannot end render pass for command buffer from a different frame");
        
        vkCmdEndRenderPass(command_buffer);

    }
}


