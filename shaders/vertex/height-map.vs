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

void main()
{
    //  float height = texture(heightMap, aTexCoords);
    vec4 texel = textureLod(heightMap, aTexCoords, 0.0);

    vec4 pos = vec4(aPos, 1.0);
    // float scale = 0.01 * (sin(0.6 * t) + 1.0);
    float scale = 0.05;
    pos.y += scale * pulseHeight * texel.x;
    gl_Position = pos;
    // gl_Position = projection * view * model * pos;
    TexCoords = aTexCoords;
}