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
#include "osc.hpp"
#include "objects.hpp"
#include "scene.hpp"


unsigned int N = 100;
unsigned int M = 100;


int main()
{
    GLFWwindow* window = sceneInit(width, height);

    camera.Position = glm::vec3(10.0f, 10.0f, 100.0f);
    camera.MovementSpeed = 20.2f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    OSCServer oscServer(37341);
    oscServer.start();

    // reverse the order of these to get some geometry glitch  ¯\_(ツ)_/¯
    Shader geometryShader("shaders/geometry.vs", "shaders/geometry.fs", "shaders/geometry.gs");
    Plane plane = Plane(N, M);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
    float pulseHeight = 0.0f;
    float scale = 100.0f;

    std::vector<MidiNoteEvent> *kickQueue = &oscServer.midiNoteQueue[2];
    std::vector<MetronomeEvent> *metronomeQueue = &oscServer.metronomeQueue;

    while(!glfwWindowShouldClose(window))
    {
        if (!metronomeQueue->empty()) {
            MetronomeEvent event = metronomeQueue->back();
            metronomeQueue->pop_back();
            pulseHeight = 1.0f;
        }

        if (!kickQueue->empty()) {
            MidiNoteEvent event = kickQueue->back();
            kickQueue->pop_back();
            std::cout << "note: " << event.note << " velocity: " << event.velocity << std::endl;
            if (event.velocity > 0) {
                pulseHeight = (float)event.velocity / 127.0f;
            }
        }

        pulseHeight *= 0.95f;//60.0f * deltaTime;

        float rotationAngle = 1.0f * t;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;
        model = glm::translate(model, plane.Position);
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));

        geometryShader.use();

        geometryShader.setFloat("t", t);
        geometryShader.setFloat("f", 10.0f);
        geometryShader.setFloat("pulseHeight", pulseHeight);

        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);
        geometryShader.setMat4("model", model);

        scenePredraw();
        plane.draw();
        scenePostdraw();
    }

    glfwTerminate();
    return 0;
}
