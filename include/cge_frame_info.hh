#pragma once

#include "cge_camera.hh"

#include <vulkan/vulkan.h>

namespace cge {

    struct FrameInfo {
        int frame_index;
        float frame_time;
        VkCommandBuffer command_buffer;
        CGE_Camera& camera;
    };

} // cge
