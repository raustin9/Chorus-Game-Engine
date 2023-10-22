#include "cge_model.hh"

#include "utils.hh"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tinyobjloader.hh>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <cassert>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace std {
    
    template<>
    struct hash<cge::CGE_Model::Vertex>{
        size_t operator()(cge::CGE_Model::Vertex const &vertex) const {
            size_t seed = 0;
            cge::hash_combine(seed, vertex.position, vertex.color, vertex.normals, vertex.uv);
            return seed;
        }
    };

} // std

namespace cge {
    CGE_Model::CGE_Model(CGE_Device &device, const CGE_Model::Builder& builder) : _device{device} {
        this->_create_vertex_buffers(builder.vertices);
        this->_create_index_buffers(builder.indices);
    }
    CGE_Model::~CGE_Model() {
        vkDestroyBuffer(_device.device(), _vertex_buffer, nullptr);
        vkFreeMemory(_device.device(), _vertex_buffer_memory, nullptr);

        if (_has_index_buffer) {
            vkDestroyBuffer(_device.device(), _index_buffer, nullptr);
            vkFreeMemory(_device.device(), _index_buffer_memory, nullptr);
        }
    }

    void
    CGE_Model::_create_index_buffers(const std::vector<uint32_t> &indices) {
        _index_count = static_cast<uint32_t>(indices.size());
        _has_index_buffer = _index_count > 0;

        if (!_has_index_buffer)
            return;

        VkDeviceSize buffer_size = sizeof(indices[0]) * _index_count;
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        _device.createBuffer(
                buffer_size, 
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                staging_buffer,
                staging_buffer_memory
            );

        void *data;
        vkMapMemory(_device.device(), staging_buffer_memory, 0, buffer_size, 0, &data);

        memcpy(data, indices.data(), 
                static_cast<size_t>(buffer_size));
        vkUnmapMemory(_device.device(), staging_buffer_memory);
        
        _device.createBuffer(
                buffer_size, 
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                _index_buffer,
                _index_buffer_memory
            );

        // Move content of the staging buffer to the index buffer
        _device.copyBuffer(staging_buffer, _index_buffer, buffer_size);

        // Free the staging buffers after use
        vkDestroyBuffer(_device.device(), staging_buffer, nullptr);
        vkFreeMemory(_device.device(), staging_buffer_memory, nullptr);
    }


    std::unique_ptr<CGE_Model> 
    CGE_Model::create_model_from_file(
        CGE_Device& device, 
        const std::string &filepath
    ) {
        Builder builder{};
        builder.load_models(filepath);

        std::cout << "Vertex Count: " << builder.vertices.size() << std::endl;

        return std::make_unique<CGE_Model>(device, builder);
    }


    void
    CGE_Model::_create_vertex_buffers(const std::vector<Vertex> &vertices) {
        this->_vertex_count = static_cast<uint32_t>(vertices.size());
        assert(this->_vertex_count >= 3 && "Vertex count must be at least 3");
        
        VkDeviceSize buffer_size = sizeof(vertices[0]) * this->_vertex_count;

        // Create the staging buffers
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        _device.createBuffer(
                buffer_size, 
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                staging_buffer,
                staging_buffer_memory
            );



        void *data;
        vkMapMemory(_device.device(), staging_buffer_memory, 0, buffer_size, 0, &data);

        memcpy(data, vertices.data(), 
                static_cast<size_t>(buffer_size));
        vkUnmapMemory(_device.device(), staging_buffer_memory);
        
        this->_device.createBuffer(
                buffer_size, 
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                _vertex_buffer,
                _vertex_buffer_memory
                );

        // Move content of the staging buffer to the index buffer
        _device.copyBuffer(staging_buffer, _vertex_buffer, buffer_size);

        // Free the staging buffers after use
        vkDestroyBuffer(_device.device(), staging_buffer, nullptr);
        vkFreeMemory(_device.device(), staging_buffer_memory, nullptr);
    }

    void
    CGE_Model::_draw(VkCommandBuffer command_buffer) {
        // If we have an index buffer, use that, otherwise, just use normal draw call
        if (_has_index_buffer) {
            vkCmdDrawIndexed(command_buffer, _index_count, 1, 0, 0, 0);
        } else {
            vkCmdDraw(command_buffer, this->_vertex_count, 1, 0, 0);
        }
    }

    void
    CGE_Model::_bind(VkCommandBuffer command_buffer) {
        VkBuffer buffers[] = {this->_vertex_buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

        if (_has_index_buffer) {
            vkCmdBindIndexBuffer(command_buffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    // Get the vertex's binding descriptions
    std::vector<VkVertexInputBindingDescription>
    CGE_Model::Vertex::get_binding_description() {
        std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
        binding_descriptions[0].binding = 0;
        binding_descriptions[0].stride = sizeof(Vertex);
        binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_descriptions;

    }

    // Get the vertex's attribute descriptions
    std::vector <VkVertexInputAttributeDescription>
    CGE_Model::Vertex::get_attribute_description() {
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, position);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        return attribute_descriptions;
    }

    // Load a model from a wavefront .obj file
    void
    CGE_Model::Builder::load_models(const std::string &filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> unique_vertices{};

        for (const auto &shape : shapes) {
            for (const auto &index : shape.mesh.indices) {
                Vertex vertex{};

                // Set the vertex position coords
                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    // Extend unofficial support for colored vertices
                    // if provided
                    auto color_index = 3 * index.vertex_index + 2;
                    if (color_index < attrib.colors.size()) {
                        vertex.color = {
                            attrib.colors[color_index - 2],
                            attrib.colors[color_index - 1],
                            attrib.colors[color_index - 0]
                        };
                    } else {
                        vertex.color = {
                            1.f,
                            1.f,
                            1.f
                        };
                    }
                }

                // Set hte vertex normal coords
                if (index.normal_index >= 0) {
                    vertex.normals = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                // Set the vertex texture coords
                if (index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 1],
                        attrib.texcoords[2 * index.texcoord_index + 0],
                    };
                }

                if (unique_vertices.count(vertex) == 0) {
                    unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(unique_vertices[vertex]);
            }
        }
    }
}
