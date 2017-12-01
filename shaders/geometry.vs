#version 330 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 vertexNormal;

//out vec3 fragPos;

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
    gl_Position = vec4(aPos.x, y, aPos.z, 1.0);

    // Normal = mat3(transpose(inverse(model))) * vertexNormal;
    //fragPos = vec3(model * vec4(aPos, 1.0));
}