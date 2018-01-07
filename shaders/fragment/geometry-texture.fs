#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

uniform float t;

uniform sampler2D texture1;

void main()
{
    float w = 0.50;
    vec3 lightDir = normalize(vec3(100.0 * sin(w*t), 20.0, 100.0 * cos(w*t)));
    // vec3 lightDir = normalize(vec3(sin(t)*100.0, min(abs(cos(t)), 0.4)*100.0 , 0.0));
    float direct = max(dot(normal, lightDir), 0.0);
    float diff = max(dot(normal, vec3(0.0, 1.0, 0.0)), 0.0);

    // FragColor = vec4(sin(fragPos.x), cos(fragPos.y), fragPos.z, fragPos.x);
    // FragColor = vec4(fragPos.x,diff, 0, 0);

    vec4 ambient = vec4(0.1, 0.1, 0.5, 1.0);
    FragColor = ambient +  direct * texture(texture1, texCoords) + 0.2 * diff * texture(texture1, texCoords);

    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}