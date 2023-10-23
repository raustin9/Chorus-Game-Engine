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

    }

    void
    CGE_Model::_create_index_buffers(const std::vector<uint32_t> &indices) {
        _index_count = static_cast<uint32_t>(indices.size());
        _has_index_buffer = _index_count > 0;

        if (!_has_index_buffer)
            return;

        VkDeviceSize buffer_size = sizeof(indices[0]) * _index_count;
        uint32_t index_size = sizeof(indices[0]);

        CGE_Buffer staging_buffer {
            _device,
            index_size,
            _index_count,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        staging_buffer.map();
        staging_buffer.write_to_buffer((void*)indices.data());

        _index_buffer = std::make_unique<CGE_Buffer>(
            _device,
            index_size,
            _index_count,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        
        // Move content of the staging buffer to the index buffer
        _device.copyBuffer(staging_buffer.get_buffer(), _index_buffer->get_buffer(), buffer_size);
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

        uint32_t vertex_size = sizeof(vertices[0]);

        CGE_Buffer staging_buffer{
            _device,
            vertex_size,
            _vertex_count,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        staging_buffer.map();
        staging_buffer.write_to_buffer((void*)vertices.data());

        _vertex_buffer = std::make_unique<CGE_Buffer>(
            _device,
            vertex_size,
            _vertex_count,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        // Move content of the staging buffer to the index buffer
        _device.copyBuffer(staging_buffer.get_buffer(), _vertex_buffer->get_buffer(), buffer_size);
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
        VkBuffer buffers[] = {this->_vertex_buffer->get_buffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

        if (_has_index_buffer) {
            vkCmdBindIndexBuffer(command_buffer, _index_buffer->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
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
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};

        attribute_descriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attribute_descriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attribute_descriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normals)});
        attribute_descriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

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
                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
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
