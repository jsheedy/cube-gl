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
#include "model.hpp"
#include "objects.hpp"
#include "scene.hpp"


unsigned int N = 200;
unsigned int M = 200;

int main()
{
    GLFWwindow* window = sceneInit(width, height);

    camera.Position = glm::vec3(10.0f, 10.0f, 100.0f);
    camera.MovementSpeed = 20.2f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    OSCServer oscServer(37341);
    // oscServer.start();

    // reverse the order of these to get some geometry glitch  ¯\_(ツ)_/¯
    Shader geometryShader("shaders/geometry.vs", "shaders/geometry.fs", "shaders/geometry.gs");
    Shader lineShader("shaders/lines.vs", "shaders/lines-blue.fs", "shaders/lines-wide.gs");
    Shader modelShader("shaders/models.vs", "shaders/models.fs", NULL);
    // Shader lineShader("shaders/lines.vs", "shaders/lines.fs", "shaders/passthru-lines.gs");
    Plane plane = Plane(N, M);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100000.0f);
    float pulseHeight = 0.0f;
    float scale = 200.0f;

    std::vector<MidiNoteEvent> *kickQueue = &oscServer.midiNoteQueue[2];
    std::vector<MetronomeEvent> *metronomeQueue = &oscServer.metronomeQueue;
    std::vector<EnvelopeEvent> *envelopeQueue = &oscServer.envelopeQueue[1];

    char *fname = (char *)"assets/nanosuit/nanosuit.obj";
    Model nanoModel(fname);

    while(!glfwWindowShouldClose(window))
    {
        if (!metronomeQueue->empty()) {
            MetronomeEvent event = metronomeQueue->back();
            metronomeQueue->pop_back();
            pulseHeight = 1.0f;
        }

        // if (!kickQueue->empty()) {
        //     MidiNoteEvent event = kickQueue->back();
        //     kickQueue->pop_back();
        //     std::cout << "note: " << event.note << " velocity: " << event.velocity << std::endl;
        //     if (event.velocity > 0) {
        //         pulseHeight = (float)event.velocity / 127.0f;
        //     }
        // }

        if (!envelopeQueue->empty()) {
            EnvelopeEvent event = envelopeQueue->back();
            envelopeQueue->pop_back();
            std::cout << "value: " << event.value << std::endl;
            pulseHeight = event.value;
        }

        pulseHeight *= 0.95f;//60.0f * deltaTime;

        float rotationAngle = 1.0f * t;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;
        model = glm::translate(model, plane.Position);
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, 50.0, scale));

        scenePredraw();

        geometryShader.use();

        geometryShader.setFloat("t", t);
        geometryShader.setFloat("f", 10.0f);
        geometryShader.setFloat("pulseHeight", pulseHeight);

        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);
        geometryShader.setMat4("model", model);

        plane.draw();

        lineShader.use();
        lineShader.setFloat("t", t);
        lineShader.setFloat("pulseHeight", pulseHeight);
        lineShader.setMat4("MVP", projection * view * model);

        // if ((int)t % 2 == 0)
        // plane.drawLines();

        // plane.drawPoints();

        // nanosuit model
        model = glm::mat4();
        scale = 1.0f;
        // model = glm::translate(model, plane.Position);
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setMat4("model", model);

        nanoModel.Draw(modelShader);

        scenePostdraw();
    }

    glfwTerminate();
    return 0;
}
