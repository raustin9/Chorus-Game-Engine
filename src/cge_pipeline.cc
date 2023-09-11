#include "cge_pipeline.hh"
#include "cge_device.hh"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vulkan/vulkan_core.h>

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
        auto vertCode = this->read_file(vertexFilepath);
        auto fragCode = this->read_file(fragmentFilepath);

        std::cout << "Vertex code size: " << vertCode.size() << std::endl;
        std::cout << "Fragment code size: " << fragCode.size() << std::endl;
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
        return config;
    }

} /* end lve namespace */
