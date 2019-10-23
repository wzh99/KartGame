#pragma once

#include "Common.hpp"

#include <memory>

class BaseApp {
public:
    BaseApp(int width, int height, const std::string &name);
    void Init();
    void Display();

protected:
    virtual void setup();
    virtual void draw();
    
    int width, height;
    std::string name;
    GLFWwindow* window;
};