#version 330 core

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D ourTexture;

out vec4 FragColor;


void main()
{
    FragColor = texture(ourTexture, TexCoords);
}
