#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

uniform float t;

void main()
{
    vec3 lightDir = normalize(vec3(10.0, 20.0, 0.0));
    // vec3 lightDir = normalize(vec3(sin(t)*100.0, min(abs(cos(t)), 0.4)*100.0 , 0.0));
    float diff = max(dot(normal, lightDir), 0.0);
    // FragColor = vec4(sin(fragPos.x), cos(fragPos.y), fragPos.z, fragPos.x);
    // FragColor = vec4(fragPos.x,diff, 0, 0);

    FragColor = vec4(
        diff * fragPos.y,
        diff * fragPos.x,
        diff * fragPos.z,
        1.0
    );
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}