#version 330 core
out vec4 FragColor;

in vec3 normal;

void main()
{
    vec3 lightDir = vec3(0.0, 1.0, 0.0);
    float diff = max(dot(normal, lightDir), 0.0);
    FragColor = vec4(diff, diff, diff, 1.0);
}