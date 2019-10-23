#include "BaseApp.hpp"

BaseApp::BaseApp(int width, int height, const std::string &name) 
    : width(width), height(height), name(name) {}

void BaseApp::Init() {
    // Initialize the library
    if (!glfwInit())
        Error("Failed to initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        Error("Failed to create window");
    }
    glfwMakeContextCurrent(window); // must do this before initializing GLEW

    // Load GL extension functions with GLEW
    glewExperimental = true;
    if (glewInit()) 
        Error("Failed to initialize GLEW");

    // Configure window
    glEnable(GL_MULTISAMPLE); // use built-in MSAA

    setup();
}

void BaseApp::setup() {}

void BaseApp::Display() {
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void BaseApp::draw() {}
