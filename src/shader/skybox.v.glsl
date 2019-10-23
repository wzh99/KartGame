#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 proj;
uniform mat4 view;

void main() {
    TexCoord = aPos;
    vec4 pos = proj * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // result in a z coordinate which is always 1.0
}  