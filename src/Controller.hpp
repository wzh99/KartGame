#pragma once

#include "Common.hpp"
#include <tuple>

class Controller {
public:
    Controller(GLFWwindow *window, glm::vec3 initPos, float initDir);
    void Update();
    glm::mat4 GetModelMatrix();
    std::pair<glm::mat4, glm::vec3> GetView(); // view matrix and camera position
};