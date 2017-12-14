#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float t;

uniform vec2 offsets[400];

void main()
{
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(aPos.x + offset.x, aPos.y, aPos.z + offset.y, 1.0);
    // gl_Position = vec4(aPos.x + offset.x + 20.0 * gl_InstanceID, aPos.y, aPos.z + offset.y, 1.0);
}