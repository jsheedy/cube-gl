#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 fragPos;
out vec3 Normal;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
  fragPos = model * vec4(aPos, 1.0f);
  // Normal = model * vertexNormal;
  // use the normal matrix to handle nonuniform scaling
  Normal = mat3(transpose(inverse(model))) * vertexNormal;
}
