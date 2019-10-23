#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

out vec3 WorldPos; // position of vertex in world space
out vec3 Norm;
out vec2 TexCoord;
out vec4 LightSpacePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normMat;
uniform mat4 lightSpaceMat;

void main() {
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Norm = normMat * aNorm;
    TexCoord = aTexCoord;
    LightSpacePos = lightSpaceMat * vec4(WorldPos, 1.0);
    gl_Position = proj * view * vec4(WorldPos, 1.0);
}
