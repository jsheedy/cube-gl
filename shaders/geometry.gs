#version 330 core

layout (points) in;
// layout (points, max_vertices = 1000) out;
layout (triangle_strip, max_vertices = 4) out;

// vec3 GetNormal()
// {
//    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
//    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
//    return normalize(cross(a, b));
// }

// vec4 explode(vec4 position, vec3 normal)
// {
//     float magnitude = 2.0;
//     vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
//     return position + vec4(direction, 0.0);
// }

void main() {

    // float N = 1000.0f;
    // for (float i=0; i<N; i++) {
    //     float l = 5.0f;
    //     float delta = -l + i/N * 2.0f*l;
    //     gl_Position = gl_in[0].gl_Position + delta;
    //     EmitVertex();
    // }

    float w = 0.005f;
    gl_Position = gl_in[0].gl_Position + vec4(-w, -w, 0.0, 0.0);    // 1:bottom-left
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( w, -w, 0.0, 0.0);    // 2:bottom-right
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(-w,  w, 0.0, 0.0);    // 3:top-left
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( w,  w, 0.0, 0.0);    // 4:top-right
    EmitVertex();

    EndPrimitive();
}