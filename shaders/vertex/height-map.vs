#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float t;
uniform float pulseHeight;
uniform sampler2D heightMap;

out vec2 texCoords;
out vec3 normal;

float height(vec2 p) {
    // return 0.5;
    vec4 texel = texture(heightMap, p);
    return texel.x;
}

void main()
{
    float h = height(aTexCoords);
    // vec4 texel = textureLod(heightMap, aTexCoords, 0.0);

    vec4 pos = vec4(aPos, 1.0);
    // float scale = 0.01 * (sin(0.6 * t) + 1.0);
    float scale = 0.05;
    pos.y += scale * pulseHeight * h;
    // gl_Position = pos;
    gl_Position = projection * view * model * pos;
    texCoords = aTexCoords;

    // calculate bogo-normal, hat tip to
    // https://stackoverflow.com/a/13983431/958118

    // # P.xy store the position for which we want to calculate the normals
    // # height() here is a function that return the height at a point in the terrain

    vec2 P = aTexCoords;
    // float delta = 0.01;
    float delta = 0.0001 + pulseHeight * 0.1;
    vec3 off = vec3(delta, delta, 0.0);
    float hL = height(P.xy - off.xz);
    float hR = height(P.xy + off.xz);
    float hD = height(P.xy - off.zy);
    float hU = height(P.xy + off.zy);

    // deduce terrain normal
    vec3 N;
    N.x = hL - hR;
    N.y = hD - hU;
    N.z = delta * 2.0;
    normal = normalize(N);

}