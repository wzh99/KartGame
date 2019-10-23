#pragma once

#include "Program.hpp"

enum TextureType {
    TEXTURE_DIFFUSE,
    TEXTURE_SPECULAR,
    NUM_TEXTURE_TYPE
};

static const std::string TexTypeStr[] = {
    "texDiffuse",
    "texSpecular",
};

struct Texture {
    GLuint Id;
    std::string Path;
    TextureType Type;
    
    static Texture FromFile(const char *path);
};

// Encapsulate sky cubemap as well as the procedure to render sky box
class Skybox {
public:
    // Name convention: prefix + direction + extension
    Skybox(const char *prefix, const char *ext);
    void Render(const glm::mat4& view, const glm::mat4 &proj); // render visible skybox
    void SetCubemap(Program &prog); // set cubemap for use in other programs

private:
    Program renderer; // skybox renderer
    GLuint cubemap; // cubemap texture
    GLuint vao, vbo; // objects for box geometry
    static float boxPos[108];
};

