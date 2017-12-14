#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;
out vec2 TexCoords;

void main()
{
 gl_Position = projection * view * model * vec4(aPos, 1.0f);
 fragPos = vec3(model * vec4(aPos, 1.0));
 // use the normal matrix to handle nonuniform scaling
 normal = mat3(transpose(inverse(model))) * vertexNormal;
 TexCoords = aTexCoords;
}
