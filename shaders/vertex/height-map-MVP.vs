#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float t;
uniform float pulseHeight;
uniform sampler2D heightMap;

out vec2 TexCoords;
out vec3 normal;

float height(vec2 p) {
    // return 0.5;
    vec4 texel = texture(heightMap, p);
    return texel.x;
}

void main()
{
    float h = height(aTexCoords);
    vec4 pos = vec4(aPos, 1.0);
    // float scale = 0.01 * (sin(0.6 * t) + 1.0);
    float scale = 0.05;
    pos.y += scale * pulseHeight * h;
    gl_Position = projection * view * model * pos;
    TexCoords = aTexCoords;
}