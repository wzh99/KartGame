#include "Texture.hpp"
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

static const std::unordered_map<int, GLenum> nCompToFormat {
        {1, GL_RED}, {3, GL_RGB}, {4, GL_RGBA}
    };

Texture Texture::FromFile(const char *path) {
    // Read texture data from image file
    int width ,height, nComp;
    auto data = stbi_load(path, &width, &height, &nComp, 0);
    if (!data) 
        Error("Failed to load texture from %s", path);

    // Load as OpenGL texture
    GLuint id;
    glGenTextures(1, &id);
    auto format = nCompToFormat.at(nComp);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    return Texture{id, std::string(path), TEXTURE_DIFFUSE};
}

Skybox::Skybox(const char *prefix, const char *ext)
    : renderer("../src/shader/skybox.v.glsl", "../src/shader/skybox.f.glsl")
{
    // Load cubemap from directory
    static const std::string dirStr[] = {
        "right", "left", "top", "bottom", "front", "back",
    };
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (auto i = 0u; i < 6; i++) {
        int width, height, nComp;
        auto filename = prefix + dirStr[i] + '.' + ext;
        auto data = stbi_load(filename.c_str(), &width, &height, &nComp, 0);
        if (!data)
            Error("Failed to load cubemap at path: %s", filename.c_str());
        auto format = nCompToFormat.at(nComp);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, format, 
            GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    // Configure cubemap parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Load box geometry
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxPos), boxPos, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
}

#define CUBEMAP_TEXTURE_INDEX 30

void Skybox::Render(const glm::mat4& view, const glm::mat4 &proj) {
    glDepthFunc(GL_LEQUAL);
    renderer.Use();
    renderer.SetMat4("view", glm::mat4(glm::mat3(view))); // remove translation in view matrix
    renderer.SetMat4("proj", proj);
    renderer.SetInt("cubemap", CUBEMAP_TEXTURE_INDEX);
    glActiveTexture(GL_TEXTURE0 + CUBEMAP_TEXTURE_INDEX);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::SetCubemap(Program &prog) {
    prog.SetInt("cubemap", CUBEMAP_TEXTURE_INDEX);
    glActiveTexture(GL_TEXTURE0 + CUBEMAP_TEXTURE_INDEX);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
}

float Skybox::boxPos[108] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};