#version 330 core
layout (location = 0) in vec3 aPos;

uniform float t;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float f = 1.0;
    float phase1 = 50.0 * aPos.x;
    float phase2 = 20.0 * aPos.z;
    float y = 0.04 * sin(f * t + phase1) + 0.03 * sin(f * t + phase2);
    // y = 0.0;
    // y = aPos.x * aPos.z;
    gl_Position = vec4(aPos.x, y, aPos.z, 1.0);
    // gl_Position = vec4(aPos.x + (sin(t+aPos.z*10)) / 10, y, aPos.z, 1.0);
}