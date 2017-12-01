#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

out vec3 normal;

void main() {
    vec3 a = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);

    normal = normalize(cross(a,b)); //vec3(1.0, 1.0, 0.0);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}