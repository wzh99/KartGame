#version 420 core

out vec4 fragColor;

in vec3 TexCoord;

uniform samplerCube cubemap;

void main() {    
    fragColor = texture(cubemap, TexCoord);
}