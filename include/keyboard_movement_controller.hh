#pragma once

#include "cge_game_object.hh"
#include "cge_window.hh"

namespace cge {

    class KeyboardMovementController {
        public:
            struct KeyMappings {
                int move_left = GLFW_KEY_A;
                int move_right = GLFW_KEY_D;
                int move_forward = GLFW_KEY_W;
                int move_backward = GLFW_KEY_S;
                int move_up = GLFW_KEY_SPACE;
                int move_down = GLFW_KEY_LEFT_SHIFT;
                int look_left = GLFW_KEY_LEFT;
                int look_right = GLFW_KEY_RIGHT;
                int look_up = GLFW_KEY_UP;
                int look_down= GLFW_KEY_DOWN;
            };

            void move_in_plane_xz(GLFWwindow* window, float dt, CGE_Game_Object& game_object);

            KeyMappings keys{};
            float move_speed{3.f};
            float look_speed{1.5f};

        private:

    };
}
