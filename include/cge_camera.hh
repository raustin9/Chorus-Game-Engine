#pragma once
#ifndef CGE_CAMERA
#define CGE_CAMERA

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_PATTERN_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace cge {
    class CGE_Camera {
        public:
            void set_orthographic_projection(
                float left,
                float right,
                float top,
                float bottom,
                float near,
                float far);
            void set_perspective_projection(
                float fovy,   // vertical field of view
                float aspect, // aspect ratio
                float near,   // near clipping plane
                float far);   // far clipping plane
            
            // Accessors
            const glm::mat4& get_projection_matrix() const { return this->_projection_matrix; }
        private:
            glm::mat4 _projection_matrix{1.f};

    };
}

#endif /* CGE_CAMERA */
