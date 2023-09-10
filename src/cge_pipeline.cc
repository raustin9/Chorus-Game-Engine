#include "cge_pipeline.hh"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace cge {

    CGE_Pipeline::CGE_Pipeline(const std::string& vertexFilepath, const std::string& fragmentFilepath) {
        this->create_graphics_pipeline(vertexFilepath, fragmentFilepath);
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
    
    void
    CGE_Pipeline::create_graphics_pipeline(const std::string& vertexFilepath, const std::string& fragmentFilepath) {
        auto vertCode = this->read_file(vertexFilepath);
        auto fragCode = this->read_file(fragmentFilepath);

        std::cout << "Vertex code size: " << vertCode.size() << std::endl;
        std::cout << "Fragment code size: " << fragCode.size() << std::endl;
    }

} /* end lve namespace */
