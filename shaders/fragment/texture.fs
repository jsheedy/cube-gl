#version 330 core

in vec2 TexCoords;
// in vec3 normal;
// in vec3 fragPos;

uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0); //texture(texture1, TexCoords);
    FragColor = texture(texture1, TexCoords);
}
