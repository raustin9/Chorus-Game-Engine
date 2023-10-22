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
#include <memory>


namespace cge {
    class CGE_Model {
        public:
            struct Vertex {
                glm::vec3 position{};
                glm::vec3 color{};
                glm::vec3 normals{};
                glm::vec2 uv{};

                static std::vector<VkVertexInputBindingDescription> get_binding_description();
                static std::vector<VkVertexInputAttributeDescription> get_attribute_description();
                bool operator==(const Vertex& other) const {
                    return position == other.position
                           && color == other.color
                           && normals == other.normals
                           && uv == other.uv;
                }
            };

            struct Builder {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};

                void load_models(const std::string& filepath);
            };

            CGE_Model(CGE_Device &device, const CGE_Model::Builder& builder);
            ~CGE_Model();
            CGE_Model(const CGE_Model&) = delete;
            CGE_Model &operator=(const CGE_Model&) = delete;

            static std::unique_ptr<CGE_Model> create_model_from_file(CGE_Device& device, const std::string &file_path);

            void _bind(VkCommandBuffer command_buffer);
            void _draw(VkCommandBuffer command_buffer);

        private:
            void _create_vertex_buffers(const std::vector<Vertex> &vertices);
            void _create_index_buffers(const std::vector<uint32_t> &indices);

            CGE_Device &_device;
            VkBuffer _vertex_buffer;
            VkDeviceMemory _vertex_buffer_memory;
            uint32_t _vertex_count;

            VkBuffer _index_buffer;
            VkDeviceMemory _index_buffer_memory;
            uint32_t _index_count;

            bool _has_index_buffer = false;
    };
}

#endif /* CGE_MODEL */
