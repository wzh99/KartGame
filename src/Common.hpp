#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

template <class...Args>
inline void Error(const char *format, Args... args) {
    char msg[256];
    std::sprintf(msg, format, args...);
    throw std::runtime_error(msg);
}