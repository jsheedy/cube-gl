#version 330 core
layout (location = 0) in vec3 aPos;

uniform float t;
uniform float f;
uniform float pulseHeight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float phase1 = 50.0 * aPos.x;
    float phase2 = 20.0 * aPos.z;
    float pulse = pulseHeight;
    float y =
        0.02 * sin(f * t + phase1)
        + 0.02 * sin(f * t + phase2);

    y *= pulseHeight * (0.5-aPos.x)/0.5 * (0.5 - aPos.z)/0.5;
    // y = 0.0;
    // y = aPos.x * aPos.z;
    gl_Position = vec4(aPos.x, y, aPos.z, 1.0);
    // gl_Position = vec4(aPos.x + (sin(t+aPos.z*10)) / 10, y, aPos.z, 1.0);
}