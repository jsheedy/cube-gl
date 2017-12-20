#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 MVP;

uniform float t;
uniform float cameraX;
uniform float cameraZ;
uniform float pulseHeight;

// uniform vec2 offsets[900];

out vec3 normal;
flat out int instanceID;

void main()
{
    float width = 200.0;
    // vec2 offset = offsets[gl_InstanceID];
    float x = width * (gl_InstanceID / 30 - 15);
    float z = width * (gl_InstanceID % 30 - 15);
    vec2 offset = vec2(x,z);

    float y = aPos.y;
    if (aPos.y < 10.0) {
       y = aPos.y * pulseHeight;
    }
    // gl_Position = projection * view * model * vec4(aPos.x + offset.x, y, aPos.z + offset.y, 1.0);
    gl_Position = MVP * vec4(aPos.x + offset.x, y, aPos.z + offset.y, 1.0);

    normal = aNormal;
    // normal = normalize(model * aNormal);
    // normal = mat3(transpose(inverse(model))) * aNormal;
    instanceID = gl_InstanceID;
}