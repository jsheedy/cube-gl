#version 330 core
//layout (location = 0) in vec2 aPos;
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 vertexNormal;

// out vec3 fragPos;

uniform float t;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float y = 0.02 * sin(2.0*t+ 20.0 * aPos.x) + 0.03 * sin(2.0*t+ 20.0 * aPos.z);
    gl_Position = projection * view * model * vec4(aPos.x, y, aPos.z, 1.0);
    // gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // fragPos = vec3(model * vec4(aPos, 1.0));
}