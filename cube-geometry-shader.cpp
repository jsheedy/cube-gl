// std stuff
#include <cmath>
#include <iostream>

// GL stuff
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// local stuff
#include "shader.h"
// #include "camera.h"
// #include "textures.h"
// #include "cube_vertices.h"
#include "osc.hpp"
#include "objects.hpp"
#include "scene.hpp"


unsigned int N = 100;
unsigned int M = 100;

float scale = 100.0f;

int main()
{
    GLFWwindow* window = sceneInit(width, height);

    OSCServer oscServer(37341);
    // oscServer.start();

    // reverse the order of these to get some geometry glitch  ¯\_(ツ)_/¯
    Shader geometryShader("shaders/geometry.vs", "shaders/geometry.fs", "shaders/geometry.gs");
    Plane plane = Plane(N, M);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
    camera.MovementSpeed = 1.2f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    float pulseHeight = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        std::cout << t << std::endl;
        if (!oscServer.metronomeQueue.empty()) {
            int rv = oscServer.metronomeQueue.back();
            oscServer.metronomeQueue.pop_back();
            std::cout << "bpm: " << rv << std::endl;
            pulseHeight = 1.0f;
        }

        pulseHeight *= 0.95f;//60.0f * deltaTime;

        float rotationAngle = 1.0f * t;

        glm::mat4 view = camera.GetViewMatrix();
        glm::vec3 pos = glm::vec3(0.0f, 0.0f,  0.0f);
        glm::mat4 model;
        model = glm::translate(model, pos);
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));

        geometryShader.use();
        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);
        geometryShader.setFloat("t", t);
        geometryShader.setFloat("f", 10.0f);
        geometryShader.setFloat("pulseHeight", pulseHeight);
        geometryShader.setMat4("model", model);

        scenePredraw();
        plane.draw();
        scenePostdraw();
    }

    glfwTerminate();
    return 0;
}
