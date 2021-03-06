#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec2 TexCoords[];

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

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

    mat4 PV = projection * view;

    gl_Position = PV * v1;
    texCoords = TexCoords[0];
    EmitVertex();

    gl_Position = PV * v2;
    texCoords = TexCoords[1];
    EmitVertex();

    gl_Position = PV * v3;
    texCoords = TexCoords[2];
    EmitVertex();

    EndPrimitive();

}