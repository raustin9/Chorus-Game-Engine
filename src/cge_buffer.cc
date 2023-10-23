#include "cge_buffer.hh"

#include <cassert>
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace cge {

    /**
     * Returns the minimum instance size required to be compatible with device's minoffsetallignment
     * @param instance_size The size of an instance
     * @param min_offset_alignment The minimum required alignment, in bytes, for the offset member
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize
    CGE_Buffer::get_allignment(VkDeviceSize instance_size, VkDeviceSize min_offset_allignment) {
        if (min_offset_allignment > 0) {
            return (instance_size + min_offset_allignment - 1)
                   & ~(min_offset_allignment - 1);
        }

        return instance_size;
    }


    CGE_Buffer::CGE_Buffer(
        CGE_Device& device,
        VkDeviceSize instance_size,
        uint32_t instance_count,
        VkBufferUsageFlags usage_flags,
        VkMemoryPropertyFlags memory_property_flags,
        VkDeviceSize min_offset_allignment
    ) : _device{device},
        _instance_size{instance_size},
        _instance_count{instance_count},
        _usage_flags{usage_flags},
        _memory_property_flags{memory_property_flags}
    {
        _alignment_size = get_allignment(instance_size, min_offset_allignment); 
        _buffer_size = _alignment_size * instance_count;
        _device.createBuffer(_buffer_size, _usage_flags, _memory_property_flags, _buffer, _memory);
    }

    CGE_Buffer::~CGE_Buffer() {
        unmap();
        vkDestroyBuffer(_device.device(), _buffer, nullptr);
        vkFreeMemory(_device.device(), _memory, nullptr);
    }

    // Map a range of memory of this buffer. If successful,
    // _mapped points to the specified buffer range
    VkResult
    CGE_Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(_buffer && _memory && "Called map on buffer before create");
        return vkMapMemory(_device.device(), _memory, offset, size, 0, &_mapped);
    }

    // Unmap a mapped memory range
    // Does not return a result as vkUnmapMemory cannot fail
    void
    CGE_Buffer::unmap() {
        if (_mapped) {
            vkUnmapMemory(_device.device(), _memory);
            _mapped = nullptr;
        }
    }

    // Copies the specified data to the mapped buffer
    // Default value writes whole buffer range
    // @param data Pointer to the data to copy
    // @param size (optional) Size of the data to copy
    // @param offset (optional) Byte offset from beginning of mapped region
    void
    CGE_Buffer::write_to_buffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
        assert(_mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(_mapped, data, _buffer_size);
        } else {
            char *mem_offset = (char*)_mapped;
            mem_offset += offset;
            memcpy(mem_offset, data, size);
        }
    }


    // Flush a memory range of the buffer to make it visible to the device
    // Only required for non-coherent memory
    // @param size (optional) size of the memory range to flush. Pass VK_WHOLE_SIZE to flush 
    //             the complete buffer range
    // @param offset (optional) Byte offset from beginning of range
    // @return result of the flush call
    VkResult
    CGE_Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.offset = offset;
        mapped_range.size = size;
        return vkFlushMappedMemoryRanges(_device.device(), 1, &mapped_range);
    }

    // Invalidate a memory range of the buffer to make it visible to the host
    VkResult
    CGE_Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory = _memory;
        mapped_range.offset = offset;
        mapped_range.size = size;
        return vkInvalidateMappedMemoryRanges(_device.device(), 1, &mapped_range);
    }

    // Create a buffer info descriptor
    // @param size (optional) size of the memory range of descriptor
    // @param offset (optional) byte offset from beginning of range
    // @return VkDescriptorBufferInfo of specified offset and range
    VkDescriptorBufferInfo
    CGE_Buffer::descriptor_info(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo {
            _buffer,
            offset,
            size
        };
    }

    // Copies "instance_size" of data to the mapped buffer at an offset of index * alignment_size
    // @param data Pointer to the data to copy
    // @param index Used in offset calculation
    void
    CGE_Buffer::write_to_index(void* data, int index) {
        write_to_buffer(data, _instance_size, index * _alignment_size);
    }


    // Flush the memory range at index * _alignment_size
    // of the buffer to make it visible to the device
    VkResult
    CGE_Buffer::flush_index(int index) {
        return flush(_alignment_size,  index * _alignment_size);
    }

    VkDescriptorBufferInfo
    CGE_Buffer::descriptor_info_for_index(int index) {
        return descriptor_info(_alignment_size, index * _alignment_size);
    }

    VkResult
    CGE_Buffer::invalidate_index(int index) {
        return invalidate(_alignment_size, index * _alignment_size);
    }

} // cge
