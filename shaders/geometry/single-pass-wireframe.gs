#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

out vec3 fragPos;
out vec3 normal;

out float vertexDistance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 v1 = model * gl_in[0].gl_Position;
    vec4 v2 = model * gl_in[1].gl_Position;
    vec4 v3 = model * gl_in[2].gl_Position;

    vec3 a = vec3(v2 - v1);
    vec3 b = vec3(v3 - v1);

    fragPos = vec3(model * gl_in[0].gl_Position);

    normal = normalize(cross(a,b));

    gl_Position = projection * view * v1;
    EmitVertex();

    gl_Position = projection * view * v2;
    EmitVertex();

    gl_Position = projection * view * v3;
    EmitVertex();

    EndPrimitive();
}