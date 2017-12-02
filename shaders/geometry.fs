#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

void main()
{
    vec3 lightDir = vec3(0.0, 1.0, 0.0);
    float diff = max(dot(normal, lightDir), 0.0);
    // FragColor = vec4(sin(fragPos.x), cos(fragPos.y), fragPos.z, fragPos.x);
    FragColor = vec4(fragPos.x,0.2, 0, 0);
}