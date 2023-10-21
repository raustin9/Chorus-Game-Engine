#include "cge_camera.hh"

#include <cassert>
#include <limits>

namespace cge {

    /// PUBLIC ///

    // Set the camera's orthographic perspective projection matrix
    void
    CGE_Camera::set_orthographic_projection(
        float left,
        float right,
        float top,
        float bottom,
        float near,
        float far) {
        _projection_matrix = glm::mat4{1.0F};
        _projection_matrix[0][0] = 2.F / (right - left);
        _projection_matrix[1][1] = 2.F / (bottom - top);
        _projection_matrix[2][2] = 1.F / (far - near);
        _projection_matrix[3][0] = -(right + left) / (right - left);
        _projection_matrix[3][1] = -(bottom + top) / (bottom - top);
        _projection_matrix[3][2] = -near / (far - near);
    }

    // Set the camera's perspective projection matrix
    void
    CGE_Camera::set_perspective_projection(
        float fovy,
        float aspect,
        float near,
        float far) {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0F);
        const float tangent_half_fovy = tan(fovy / 2.F);

        _projection_matrix = glm::mat4{0.0F};
        _projection_matrix[0][0] = 1.F / (aspect * tangent_half_fovy);
        _projection_matrix[1][1] = 1.F / (tangent_half_fovy);
        _projection_matrix[2][2] = far / (far - near);
        _projection_matrix[2][3] = 1.F;
        _projection_matrix[3][2] = -(far * near) / (far - near);
    }


    void CGE_Camera::set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
      
        const glm::vec3 w{glm::normalize(direction)};
        const glm::vec3 u{glm::normalize(glm::cross(w, up))};
        const glm::vec3 v{glm::cross(w, u)};
    
        _view_matrix = glm::mat4{1.f};
        _view_matrix[0][0] = u.x;
        _view_matrix[1][0] = u.y;
        _view_matrix[2][0] = u.z;
        _view_matrix[0][1] = v.x;
        _view_matrix[1][1] = v.y;
        _view_matrix[2][1] = v.z;
        _view_matrix[0][2] = w.x;
        _view_matrix[1][2] = w.y;
        _view_matrix[2][2] = w.z;
        _view_matrix[3][0] = -glm::dot(u, position);
        _view_matrix[3][1] = -glm::dot(v, position);
        _view_matrix[3][2] = -glm::dot(w, position);
    }
    
    void CGE_Camera::set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
        set_view_direction(position, target - position, up);
    }
    
    void CGE_Camera::set_view_xyz(glm::vec3 position, glm::vec3 rotation) {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
        const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
        const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
        _view_matrix = glm::mat4{1.f};
        _view_matrix[0][0] = u.x;
        _view_matrix[1][0] = u.y;
        _view_matrix[2][0] = u.z;
        _view_matrix[0][1] = v.x;
        _view_matrix[1][1] = v.y;
        _view_matrix[2][1] = v.z;
        _view_matrix[0][2] = w.x;
        _view_matrix[1][2] = w.y;
        _view_matrix[2][2] = w.z;
        _view_matrix[3][0] = -glm::dot(u, position);
        _view_matrix[3][1] = -glm::dot(v, position);
        _view_matrix[3][2] = -glm::dot(w, position);
    }
    
    // PRIVATE ///

} // namespace cge
