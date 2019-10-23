#pragma once

#include "Program.hpp"
#include "Model.hpp"

class ShadowMap {
public:
    ShadowMap(glm::vec3 lightDir);
    void Render(const std::vector<std::pair<Model*, glm::mat4>> &targets);
    void SetMap(Program &prog);

private:
    Program mapper;
    GLuint fbo, tex;
    glm::mat4 lightSpaceMat;
};