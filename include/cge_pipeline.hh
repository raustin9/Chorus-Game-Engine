#pragma once
#include <vulkan/vulkan_core.h>
#ifndef LVE_PIPELINE
#define LVE_PIPELINE

#include "cge_device.hh"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace cge {

    struct PipelineConfigInfo {
        VkPipelineInputAssemblyStateCreateInfo _input_assembly_info;
        VkViewport _viewport;
        VkRect2D _scissor;
        VkPipelineRasterizationStateCreateInfo _rasterization_info;
        VkPipelineMultisampleStateCreateInfo _multisample_info;
        VkPipelineColorBlendAttachmentState _color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo _color_blend_info;
        VkPipelineDepthStencilStateCreateInfo _depth_stencil_info;
        VkPipelineLayout _pipeline_layout = nullptr;
        VkRenderPass _render_pass = nullptr;
        uint32_t _subpass = 0;
    };

    class CGE_Pipeline {
        public:
            CGE_Pipeline(
                CGE_Device &device,
                const std::string& vertexFilepath, 
                const std::string& fragmentFilepath,
                const PipelineConfigInfo &config
            );
            CGE_Pipeline(const CGE_Pipeline&) = delete;
            void operator=(CGE_Pipeline&) = delete;
            ~CGE_Pipeline();

            static PipelineConfigInfo _default_pipeline_config_info(uint32_t width, uint32_t height);

	    private:
            CGE_Device &_device;
            VkPipeline _graphics_pipeline;
            VkShaderModule _vert_shader_module;
            VkShaderModule _frag_shader_module;

	        static std::vector<char> read_file(const std::string& filepath);
	        void create_graphics_pipeline(const std::string& vertexFilepath, const std::string& fragmentFilepath, const PipelineConfigInfo configInfo);
            void _create_shader_module(const std::vector <char>& code, VkShaderModule *module);
    };

} /* end lve namespace */

#endif /* LVE_PIPELINE */
