
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifndef UTILS_HPP
#define UTILS_HPP

glm::vec3 UpVector(0.0f, 1.0f, 0.0f);
glm::vec3 RightVector(1.0f, 0.0f, 0.0f);
glm::vec3 ForwardVector(0.0f, 0.0f, 1.0f);

glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);

float angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 origin) {
    glm::vec3 da=glm::normalize(a-origin);
    glm::vec3 db=glm::normalize(b-origin);
    return glm::acos(glm::dot(da, db));
}

#endif