#version 330 core
layout (location = 0) in vec3 aPos;

uniform float t;
uniform float pulseHeight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float f = 200.0;
    float y = 2.0 * pulseHeight * abs(sin(f* aPos.x + t) * sin(f * aPos.z + t));
    gl_Position = projection * view * model * vec4(aPos.x, y, aPos.z, 1.0);
}