#include "Controller.hpp"
#include "ShadowMap.hpp"
#include <bitset>

enum ViewMode {
    VIEW_SPECTATOR, // default mode, view car and track from a fixed point
    VIEW_DRIVER, // driver's view
    VIEW_BACK, // view from back of the car
    NUM_VIEW_MODE
};

static glm::vec3 pos;
static float dir, velo;
static ViewMode mode;
static std::bitset<512> pressed;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
        pressed.set(key, true);
    if (action == GLFW_RELEASE) {
        pressed.reset(key);
        if (key == GLFW_KEY_T) 
            mode = ViewMode((mode + 1) % NUM_VIEW_MODE);
    }
}

Controller::Controller(GLFWwindow *window, glm::vec3 initPos, float initDir) {
    // Initialize parameters
    pos = initPos;
    dir = glm::radians(dir);
    velo = 0.f;
    mode = VIEW_SPECTATOR;
    glfwSetKeyCallback(window, keyCallback);
}

void Controller::Update() {
    // Compute acceleration
    constexpr auto accel = 1e-5f;
    if (pressed.test(GLFW_KEY_W)) 
        velo += accel;
    if (pressed.test(GLFW_KEY_S))
        velo -= accel;
    constexpr auto kDir = 1e-2f;
    if (pressed.test(GLFW_KEY_A)) 
        dir += kDir * std::cbrt(velo);
    if (pressed.test(GLFW_KEY_D))
        dir -= kDir * std::cbrt(velo);
    constexpr auto brake = 2e-5f;
    if (pressed.test(GLFW_KEY_SPACE)) 
        velo = std::abs(velo) <= brake ? 0 : velo - glm::sign(velo) * brake;

    // Update configuration
    constexpr auto fric = 3e-6f; // consider ground friction
    velo = std::abs(velo) <= fric ? 0 : velo -  glm::sign(velo) * fric;
    auto veloVec = velo * glm::vec3(std::sin(dir), 0, std::cos(dir));
    pos += veloVec;
}

glm::mat4 Controller::GetModelMatrix() {
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, pos);
    model = glm::rotate(model, dir, {0, 1, 0});
    return model;
}

std::pair<glm::mat4, glm::vec3> Controller::GetView() {
    glm::mat4 mat;
    glm::vec3 camPos;

    switch (mode) {
        case VIEW_SPECTATOR: {
            camPos = glm::vec3(-20, 15, 0);
            mat = glm::lookAt(camPos, {0, 0, 0}, {0, 1, 0});
            break;
        }

        case VIEW_DRIVER: {
            auto dirVec =  glm::vec3(std::sin(dir), 0, std::cos(dir));
            camPos = pos + glm::vec3(0, 0.38, 0);
            mat = glm::translate(glm::identity<glm::mat4>(), {0.2, 0, 0}); // driver seat is on the left
            mat = mat * glm::lookAt(camPos, camPos + dirVec, {0, 1, 0});
            break;
        }

        case VIEW_BACK: {
            auto camDir = dir + glm::radians(180.f);
            constexpr auto camDist = 6.4f;
            camPos = glm::vec3(std::sin(camDir) * camDist, 1, std::cos(camDir) * camDist) + pos;
            mat = glm::lookAt(camPos, pos, {0, 1, 0});
            break;
        }
    }

    return std::make_pair(mat, camPos);
}
