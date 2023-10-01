#pragma once
#ifndef SIMPLE_RENDER_SYSTEM 
#define SIMPLE_RENDER_SYSTEM 

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "cge_device.hh"
#include "cge_camera.hh"
#include "cge_pipeline.hh"
#include "cge_game_object.hh"

namespace cge {
    class SimpleRenderSystem {
        public:
            SimpleRenderSystem(CGE_Device &device, VkRenderPass render_pass);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem&) = delete;
            SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
            void render_game_objects(
                    VkCommandBuffer command_buffer, 
                    std::vector<CGE_Game_Object> &game_objects,
                    const CGE_Camera& camera);
            
        private:
            void _create_pipeline_layout();
            void _create_pipeline(VkRenderPass render_pass);

            CGE_Device& _device;

            VkPipelineLayout _pipeline_layout;
            std::unique_ptr<CGE_Pipeline> _pipeline;
    };
}

#endif /* SIMPLE_RENDER_SYSTEM */
