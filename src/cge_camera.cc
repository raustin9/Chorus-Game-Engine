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
    
    // PRIVATE ///

} // namespace cge
