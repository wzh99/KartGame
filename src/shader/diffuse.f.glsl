#version 420 core

in vec3 Norm;
in vec3 WorldPos;
in vec2 TexCoord;
in vec4 LightSpacePos;

out vec4 fragColor;

uniform mat4 view;
uniform vec3 camPos;
uniform vec3 lightDir; // in world space

uniform sampler2D texDiffuse0;
uniform sampler2D shadowMap;

float inShadow() {
    vec3 projCoord = LightSpacePos.xyz / LightSpacePos.w;
    projCoord = 0.5 * projCoord + 0.5; // transform to NDC space
    float curDepth = projCoord.z;
    if (curDepth > 1.0) return 0.0;
    if (projCoord.x < 0 || projCoord.x > 1)
        return 0.0;
    if (projCoord.y < 0 || projCoord.y > 1)
        return 0.0;
    float mapDepth = texture(shadowMap, projCoord.xy).r;
    float bias = max(1e-2 * (1.0 - dot(Norm, lightDir)), 1e-3); // resolve shadow acene
    float shadow = 0.0; // PCF
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoord.xy + vec2(x, y) * texelSize).r; 
            shadow += curDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

void main() {
    vec3 wi = normalize(lightDir);
    vec3 wo = camPos - WorldPos;
    vec3 n = normalize(Norm);
    vec3 tex = vec3(texture(texDiffuse0, TexCoord));
    vec3 amb = 0.2 * tex;
    vec3 diff = 0.8 * tex * max(dot(wi, n), 0);
    fragColor = vec4(amb + diff * (1.0 - inShadow()), 1);
}
