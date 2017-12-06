#version 330 core
layout (lines) in;
layout (triangle_strip, max_vertices=10) out;

uniform float t;

uniform mat4 MVP;

void main() {
    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;
    vec3 diff = normalize(vec3(p2) - vec3(p1));

    vec3 up = normalize(vec3(1.0, 1.0, 0.0)); // guaranteed orthogal to our grid

    vec3 norm = normalize(cross(diff, up));
    vec3 norm2 = normalize(cross(norm, diff));
    float width = 0.004 * (sin(2.0*t)/2 + 0.521);

    gl_Position = MVP * (gl_in[0].gl_Position + width * vec4(norm, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[0].gl_Position - width * vec4(norm, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[1].gl_Position - width * vec4(norm, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[1].gl_Position + width * vec4(norm, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[0].gl_Position + width * vec4(norm, 1));
    EmitVertex();

    gl_Position = MVP * (gl_in[0].gl_Position + width * vec4(norm2, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[0].gl_Position - width * vec4(norm2, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[1].gl_Position - width * vec4(norm2, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[1].gl_Position + width * vec4(norm2, 1));
    EmitVertex();
    gl_Position = MVP * (gl_in[0].gl_Position + width * vec4(norm2, 1));
    EmitVertex();

    EndPrimitive();
}