#pragma once
#ifndef CGE_GAME_OBJECT
#define CGE_GAME_OBJECT

#include "cge_model.hh"
#include <memory>

namespace cge {

    struct Transform2dComponent {
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rot_matrix{ {c, s}, {-s, c} };
            glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
            return rot_matrix * scaleMat; 
        }
    };

    class CGE_Game_Object {
        public:
            using id_t  = unsigned int;

            static CGE_Game_Object _create_game_object() {
                static id_t current_id = 0;
                return CGE_Game_Object{current_id++};
            }

            CGE_Game_Object(const CGE_Game_Object&) = delete;
            CGE_Game_Object& operator = (const CGE_Game_Object&) = delete;
            CGE_Game_Object(CGE_Game_Object&&) = default;
            CGE_Game_Object& operator = (CGE_Game_Object&&) = default;

            id_t _get_id() {return this->_id;}

            Transform2dComponent transform2d{};
            std::shared_ptr<CGE_Model> model{};
            glm::vec3 color{};

        private:
            CGE_Game_Object(id_t id) : _id(id) {}

            id_t _id;
    };
}

#endif /* CGE_GAME_OBJECT */
