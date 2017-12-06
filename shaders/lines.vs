#version 330 core
layout (location = 0) in vec3 aPos;

uniform float t;
uniform float pulseHeight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    float f = 100.0;
    float y = 1.0 * pulseHeight * abs(sin(f* aPos.x + t) * sin(f * aPos.z + t));

    float A = -0.5;
    float e = 2.71828;
    float sigmaSq = 0.01;
    float x0 = sin(t) / 10.0;
    float z0 = cos(t) / 10.0;
    float term1 = pow(aPos.x - x0, 2.0)/(2.0*sigmaSq);
    float term2 = pow(aPos.z - z0, 2.0)/(2.0*sigmaSq);
    float gaussian = A * pow(e, -(term1 + term2));

    gl_Position = projection * view * model * vec4(aPos.x, gaussian, aPos.z, 1.0);
}