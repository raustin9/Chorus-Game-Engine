#pragma once

#include "cge_device.hh"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace cge {

    class CGE_Buffer {
        public:
            CGE_Buffer(
                CGE_Device& device,
                VkDeviceSize instance_size,
                uint32_t instance_count,
                VkBufferUsageFlags flags,
                VkMemoryPropertyFlags memory_property_flags,
                VkDeviceSize min_offset_allignment = 1
            );
            ~CGE_Buffer();

            CGE_Buffer(const CGE_Buffer&) = delete;
            CGE_Buffer& operator= (const CGE_Buffer&) = delete;

            VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            void unmap();

            void write_to_buffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

            void write_to_index(void* data, int index);
            VkResult flush_index(int index);
            VkDescriptorBufferInfo descriptor_info_for_index(int index);
            VkResult invalidate_index(int index);

            VkBuffer get_buffer() const { return _buffer; }
            void* get_mapped_memory() const { return _mapped; }
            uint32_t get_instance_count() const { return _instance_count; }
            VkDeviceSize get_instance_size() const { return _instance_size; }
            VkDeviceSize get_allignment_size() const { return _alignment_size; }
            VkBufferUsageFlags get_usage_flags() const { return _usage_flags; }
            VkMemoryPropertyFlags get_memory_property_flags() const { return _memory_property_flags; }
            VkDeviceSize get_buffer_size() const { return _buffer_size; }

        private:
            static VkDeviceSize get_allignment(VkDeviceSize instance_size, VkDeviceSize min_offset_allignment);

            CGE_Device& _device;
            void* _mapped = nullptr;
            VkBuffer _buffer = VK_NULL_HANDLE;
            VkDeviceMemory _memory = VK_NULL_HANDLE;

            VkDeviceSize _buffer_size;
            uint32_t _instance_count;
            VkDeviceSize _instance_size;
            VkDeviceSize _alignment_size;
            VkBufferUsageFlags _usage_flags;
            VkMemoryPropertyFlags _memory_property_flags;
    };

} // cge
