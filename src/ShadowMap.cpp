#include "ShadowMap.hpp"

constexpr auto mapWidth = 2000, mapHeight = 4000;

ShadowMap::ShadowMap(glm::vec3 lightDir) 
    : mapper("../src/shader/shadow.v.glsl", "../src/shader/shadow.f.glsl") 
{
    // Generate depth map texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapWidth, mapHeight, 0, 
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    const float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Generate frame buffer object
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
    glDrawBuffer(GL_NONE); // don't render any color data
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    // Configure matrices
    auto proj = glm::ortho(-10.f, 10.f, -25.f, 25.f, 0.f, 30.f);
    auto view = glm::lookAt(lightDir * 10.f, {0, 0, 0}, {0, 1, 0});
    lightSpaceMat = proj * view;
}

void ShadowMap::Render(const std::vector<std::pair<Model*, glm::mat4>> &targets) {
    // Bind shadow map framebuffer
    glViewport(0, 0, mapWidth, mapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Render targets
    mapper.Use();
    mapper.SetMat4("lightSpaceMat", lightSpaceMat);
    for (auto &pair : targets)
        pair.first->Draw(mapper, pair.second);

    // Restore states
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#define SHADOWMAP_TEXTURE_INDEX 31

void ShadowMap::SetMap(Program &prog) {
    constexpr auto mapIdx = 31;
    prog.Use();
    prog.SetMat4("lightSpaceMat", lightSpaceMat);
    prog.SetInt("shadowMap", SHADOWMAP_TEXTURE_INDEX);
    glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_INDEX);
    glBindTexture(GL_TEXTURE_2D, tex);
}