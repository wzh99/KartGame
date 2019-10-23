#include "program.hpp"
#include <sstream>

Program::Program(const char *vertPath, const char *fragPath) {
    // Read shader code from file
    std::ifstream vertFile(vertPath), fragFile(fragPath);
    if (!vertFile) 
        Error("Failed to open vertex shader source: %s", vertPath);
    if (!fragFile)
       Error("Failed to open fragment shader source: %s", fragPath); 
    std::stringstream vertStream, fragStream;
    vertStream << vertFile.rdbuf();
    fragStream << fragFile.rdbuf();
    auto vertStr = vertStream.str(); // must keep original string on stack
    auto vertCStr = vertStr.c_str();
    auto fragStr = fragStream.str();
    auto fragCStr = fragStr.c_str();

    // Compile shaders
    auto vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertCStr, nullptr);
    glCompileShader(vertShader);
    checkCompileError(vertShader, "vertex");
    auto fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCStr, nullptr);
    glCompileShader(fragShader);
    checkCompileError(fragShader, "fragment");

    // Link shader program
    id = glCreateProgram();
    glAttachShader(id, vertShader);
    glAttachShader(id, fragShader);
    glLinkProgram(id);
    GLint success;
    static constexpr auto BUFFER_SIZE = 256;
    GLchar info[BUFFER_SIZE];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, BUFFER_SIZE, nullptr, info);
        Error("Failed to link program\n%s", info);
    }
}

void Program::checkCompileError(GLuint shaderId, const char *name) {
    GLint success;
    static constexpr auto BUFFER_SIZE = 256;
    GLchar info[BUFFER_SIZE];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, BUFFER_SIZE, nullptr, info);
        Error("Failed to compile %s shader\n%s", name, info);
    }
}

void Program::Use() { glUseProgram(id); }

void Program::SetInt(const char *name, int val) {
    glUniform1i(glGetUniformLocation(id, name), val);
}

void Program::SetFloat(const char *name, float val) {
    glUniform1f(glGetUniformLocation(id, name), val);
}

void Program::SetVec3(const char *name, const glm::vec3 &val) {
    glUniform3fv(glGetUniformLocation(id, name), 1, &val[0]);
}

void Program::SetMat3(const char *name, const glm::mat3 &val) {
    glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, &val[0][0]);
}

void Program::SetMat4(const char *name, const glm::mat4 &val) {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &val[0][0]);
}
