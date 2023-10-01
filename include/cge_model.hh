#pragma once
#ifndef CGE_MODEL
#define CGE_MODEL

#include "cge_device.hh"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_PATTERN_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>


namespace cge {
    class CGE_Model {
        public:
            struct Vertex {
                glm::vec3 position;
                glm::vec3 color;
                static std::vector<VkVertexInputBindingDescription> get_binding_description();
                static std::vector<VkVertexInputAttributeDescription> get_attribute_description();
            };

            CGE_Model(CGE_Device &device, const std::vector<Vertex> &vertices);
            ~CGE_Model();
            CGE_Model(const CGE_Model&) = delete;
            CGE_Model &operator=(const CGE_Model&) = delete;


            void _bind(VkCommandBuffer command_buffer);
            void _draw(VkCommandBuffer command_buffer);

        private:
            void _create_vertex_buffers(const std::vector<Vertex> &vertices);

            CGE_Device &_device;
            VkBuffer _vertex_buffer;
            VkDeviceMemory _vertex_buffer_memory;
            uint32_t _vertex_count;
    };
}

#endif /* CGE_MODEL */
