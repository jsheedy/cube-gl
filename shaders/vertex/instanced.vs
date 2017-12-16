#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float t;
uniform float cameraX;
uniform float cameraZ;
uniform float pulseHeight;

uniform vec2 offsets[900];

out vec3 normal;
flat out int instanceID;

void main()
{
    vec2 offset = offsets[gl_InstanceID];
    float y = aPos.y;
    if (aPos.y < 10.0) {
       y = aPos.y * pulseHeight;
    }
    gl_Position = projection * view * model * vec4(aPos.x + offset.x, y, aPos.z + offset.y, 1.0);


    // normal = normalize(model * aNormal);
    normal = mat3(transpose(inverse(model))) * aNormal;
    instanceID = gl_InstanceID;
}