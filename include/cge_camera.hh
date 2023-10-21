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
 
            void set_view_direction(
                glm::vec3 position,
                glm::vec3 direction,
                glm::vec3 up = glm::vec3{0.f, -1.f, 0.f}); // default the up direction to negative y axis
            
            void set_view_target( // will always be the center of the camera
                glm::vec3 position,
                glm::vec3 target,
                glm::vec3 up = glm::vec3{0.f, -1.f, 0.f}); 

            void set_view_xyz(
                glm::vec3 position,
                glm::vec3 rotation);


            // Accessors
            const glm::mat4& get_projection_matrix() const { return this->_projection_matrix; }
            const glm::mat4& get_view_matrix() const { return _view_matrix; }
        private:
            glm::mat4 _projection_matrix{1.f};
            glm::mat4 _view_matrix{1.f}; // used to store the camera transform

    };
}

#endif /* CGE_CAMERA */
