#pragma once

#include "BaseApp.hpp"
#include "Controller.hpp"
#include "ShadowMap.hpp"

class KartGame : public BaseApp {
public:
    KartGame(int width, int height);

private:
    void setup() override;
    void draw() override;

    std::unique_ptr<Model> car, track, terrain, cone;
    std::vector<glm::mat4> coneModelMats;
    std::unique_ptr<Program> diffuse, specular, trackShader;
    std::unique_ptr<Controller> control;
    glm::vec3 lightDir;
    std::unique_ptr<ShadowMap> shadow;
    std::unique_ptr<Skybox> sky;
};