#pragma once

#include "Common.hpp"
#include <fstream>

class Program {
public:
    Program(const char *vertPath, const char *fragPath);
    void Use();
    void SetInt(const char *name, int val);
    void SetFloat(const char *name, float val);
    void SetVec3(const char *name, const glm::vec3 &val);
    void SetMat3(const char *name, const glm::mat3 &val);
    void SetMat4(const char *name, const glm::mat4 &val);

private:
    void checkCompileError(GLuint shaderId, const char *name);

    GLuint id;
};