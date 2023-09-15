#include "cge_pipeline.hh"
#include "cge_model.hh"
#include "cge_device.hh"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <vulkan/vulkan.h>

namespace cge {

    CGE_Pipeline::CGE_Pipeline(
        CGE_Device &device,
        const std::string& vertexFilepath, 
        const std::string& fragmentFilepath,
        const PipelineConfigInfo &configInfo
    ) : _device{device} {
        this->create_graphics_pipeline(vertexFilepath, fragmentFilepath, configInfo);
    }

    CGE_Pipeline::~CGE_Pipeline() {
        vkDestroyShaderModule(this->_device.device(), this->_vert_shader_module, nullptr);
        vkDestroyShaderModule(this->_device.device(), this->_frag_shader_module, nullptr);

        vkDestroyPipeline(this->_device.device(), this->_graphics_pipeline, nullptr);
    }
    
    std::vector<char> 
    CGE_Pipeline::read_file(const std::string& filepath) {
        std::ifstream file{filepath, std::ios::ate | std::ios::binary};
      
        if (!file.is_open()) {
          throw std::runtime_error("failed to open file: " + filepath);
        }
      
        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);
      
        file.seekg(0);
        file.read(buffer.data(), file_size);
        file.close();
      
        return buffer;
    }
    
    //
    // Create the graphics pipeline
    //
    void
    CGE_Pipeline::create_graphics_pipeline(
        const std::string& vertexFilepath, 
        const std::string& fragmentFilepath, 
        const PipelineConfigInfo configInfo
    ) {
        assert(configInfo._pipeline_layout != VK_NULL_HANDLE 
                && "Cannot create graphics pipeline:: no pipeline_layout provided in configInfo");
        assert(configInfo._render_pass != VK_NULL_HANDLE 
                && "Cannot create graphics pipeline:: no render_pass provided in configInfo");
        auto vert_code = this->read_file(vertexFilepath);
        auto frag_code = this->read_file(fragmentFilepath);

        this->_create_shader_module(vert_code, &this->_vert_shader_module);
        this->_create_shader_module(frag_code, &this->_frag_shader_module);

        VkPipelineShaderStageCreateInfo shader_stages[2];

        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module = this->_vert_shader_module;
        shader_stages[0].pName = "main";
        shader_stages[0].flags = 0;
        shader_stages[0].pNext = nullptr;
        shader_stages[0].pSpecializationInfo = nullptr;

        shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].module = this->_frag_shader_module;
        shader_stages[1].pName = "main";
        shader_stages[1].flags = 0;
        shader_stages[1].pNext = nullptr;
        shader_stages[1].pSpecializationInfo = nullptr;

        auto binding_descriptions = CGE_Model::Vertex::get_binding_description();
        auto attribute_descriptions = CGE_Model::Vertex::get_attribute_description();
        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());;
        vertex_input_info.vertexBindingDescriptionCount   = static_cast<uint32_t>(binding_descriptions.size());
        vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();
        vertex_input_info.pVertexBindingDescriptions   = binding_descriptions.data();

        /* Specific GPU Viewport Tooling -- some GPUs can support multiple viewports and scissors. We only want 1 */
        VkPipelineViewportStateCreateInfo viewport_info{};
        viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_info.viewportCount = 1;
        viewport_info.pViewports    = &configInfo._viewport;
        viewport_info.scissorCount  = 1;
        viewport_info.pScissors     = &configInfo._scissor;

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &configInfo._input_assembly_info;
        pipeline_info.pViewportState = &viewport_info;
        pipeline_info.pRasterizationState = &configInfo._rasterization_info;
        pipeline_info.pMultisampleState = &configInfo._multisample_info;
        pipeline_info.pColorBlendState = &configInfo._color_blend_info;
        pipeline_info.pDepthStencilState = &configInfo._depth_stencil_info;
        pipeline_info.pDynamicState = nullptr; // optional

        pipeline_info.layout = configInfo._pipeline_layout;
        pipeline_info.renderPass = configInfo._render_pass;
        pipeline_info.subpass = configInfo._subpass;

        pipeline_info.basePipelineIndex = -1;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(this->_device.device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &_graphics_pipeline)
            != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to create graphics pipeline\n");
        }
    }

    void
    CGE_Pipeline::_bind(VkCommandBuffer command_buffer) {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->_graphics_pipeline);
    }


    //
    // Create a shader module
    //
    void
    CGE_Pipeline::_create_shader_module(
        const std::vector <char> &code,
        VkShaderModule *module
    ) {
        VkShaderModuleCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(this->_device.device(), &create_info, nullptr, module) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to create shader module");
        }
    }

    //
    // Default Pipeline Config
    //
    PipelineConfigInfo
    CGE_Pipeline::_default_pipeline_config_info(uint32_t width, uint32_t height) {
        PipelineConfigInfo config {};

        config._input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config._input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config._input_assembly_info.primitiveRestartEnable = VK_FALSE;

        /* Viewport Configuration */
        config._viewport.x = 0.0F;
        config._viewport.y = 0.0F;
        config._viewport.width  = static_cast<float>(width);
        config._viewport.height = static_cast<float>(height);
        config._viewport.minDepth = 0.0F;
        config._viewport.maxDepth = 1.0F;

        /* Scissor Configuration */
        config._scissor.offset = {0, 0};
        config._scissor.extent = {width, height};

        /* Rasterization Configuration */
        config._rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config._rasterization_info.depthClampEnable = VK_FALSE;
        config._rasterization_info.rasterizerDiscardEnable = VK_FALSE;
        config._rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
        config._rasterization_info.lineWidth = 1.0F;
        config._rasterization_info.cullMode = VK_CULL_MODE_NONE;
        config._rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        config._rasterization_info.depthBiasEnable = VK_FALSE;
        config._rasterization_info.depthBiasConstantFactor = 0.0F;
        config._rasterization_info.depthBiasClamp = 0.0F;
        config._rasterization_info.depthBiasSlopeFactor = 0.0F;

        /* Multisampling Configuration -- helps with aliasing */
        config._multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config._multisample_info.sampleShadingEnable   = VK_FALSE;
        config._multisample_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        config._multisample_info.minSampleShading      = 1.0F;
        config._multisample_info.pSampleMask           = nullptr;
        config._multisample_info.alphaToCoverageEnable = VK_FALSE;
        config._multisample_info.alphaToOneEnable      = VK_FALSE;

        /* Color Blending Configuration */
        config._color_blend_attachment.colorWriteMask = 
              VK_COLOR_COMPONENT_R_BIT 
            | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT
            | VK_COLOR_COMPONENT_A_BIT;
        config._color_blend_attachment.blendEnable = VK_FALSE;
        config._color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        config._color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        config._color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        config._color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        config._color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        config._color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        config._color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config._color_blend_info.logicOpEnable = VK_FALSE;
        config._color_blend_info.logicOp = VK_LOGIC_OP_COPY;
        config._color_blend_info.attachmentCount = 1;
        config._color_blend_info.pAttachments = &config._color_blend_attachment;
        config._color_blend_info.blendConstants[0] = 0.0F;
        config._color_blend_info.blendConstants[1] = 0.0F;
        config._color_blend_info.blendConstants[2] = 0.0F;
        config._color_blend_info.blendConstants[3] = 0.0F;

        /* Depth Testing Configuration */
        config._depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config._depth_stencil_info.depthTestEnable = VK_TRUE;
        config._depth_stencil_info.depthWriteEnable = VK_TRUE;
        config._depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
        config._depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        config._depth_stencil_info.minDepthBounds = 0.0F;
        config._depth_stencil_info.maxDepthBounds = 1.0F;
        config._depth_stencil_info.stencilTestEnable = VK_FALSE;
        config._depth_stencil_info.front = {};
        config._depth_stencil_info.back  = {};

        return config;
    }

} /* end lve namespace */
