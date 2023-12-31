#pragma once
#ifndef CGE_GAME_OBJECT
#define CGE_GAME_OBJECT

#include "cge_model.hh"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace cge {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
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

            TransformComponent transform{};
            std::shared_ptr<CGE_Model> model{};
            glm::vec3 color{};

        private:
            CGE_Game_Object(id_t id) : _id(id) {}

            id_t _id;
    };
}

#endif /* CGE_GAME_OBJECT */
