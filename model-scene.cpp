// std stuff
#include <cmath>
#include <iostream>
#include <sstream>

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

    camera.Position = glm::vec3(0.0f, 100.0f, 1000.0f);
    camera.MovementSpeed = 20.0f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    OSCServer oscServer(37341);
    // oscServer.start();

    // reverse the order of these to get some geometry glitch  ¯\_(ツ)_/¯
    Shader geometryShader("shaders/vertex/geometry.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    Shader cityShader("shaders/vertex/instanced.vs", "shaders/fragment/city.fs", NULL);
    Shader bunnyShader("shaders/vertex/passthru.vs", "shaders/fragment/bunny.fs", "shaders/geometry/geometry.gs");
    Shader bunnyLineShader("shaders/vertex/MVP.vs", "shaders/fragment/lines-blue.fs", NULL);
    Shader lineShader("shaders/vertex/lines.vs", "shaders/fragment/lines-blue.fs", "shaders/geometry/lines-wide.gs");
    Shader modelShader("shaders/vertex/models.vs", "shaders/fragment/models.fs", NULL);
    // Shader lineShader("shaders/lines.vs", "shaders/lines.fs", "shaders/passthru-lines.gs");
    Plane plane = Plane(N, M);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100000.0f);
    float pulseHeight = 0.0f;
    float scale = 200.0f;

    std::vector<MidiNoteEvent> *midiNoteQueue = &oscServer.midiNoteQueue[1];
    std::vector<MetronomeEvent> *metronomeQueue = &oscServer.metronomeQueue;
    std::vector<EnvelopeEvent> *envelopeQueue = &oscServer.envelopeQueue[1];

    Model nanoModel((char *)"assets/nanosuit/nanosuit.obj");
    Model velotronModel((char *)"assets/velotron_arise_eecc.fbx");
    Model cityModel((char *)"assets/SciFi_HumanCity_Kit05-OBJ.obj");
    Model bunnyModel((char *)"assets/bunny.ply");
    Model sphereModel((char *)"assets/sphere.ply");

    // glm::vec2 offsets[600];
    cityShader.use();
    unsigned int idx = 0;
    float w = 200.0;
    for (float x=-15; x<15; x+=1) {
        for (float z=-15; z<15; z+=1) {
            std::stringstream ss;
            string index;
            ss << ++idx;
            index = ss.str();
            string label = ("offsets[" + index + "]").c_str();
            cityShader.setVec2(label, x*w, z*w);
        }
    }

    while(!glfwWindowShouldClose(window))
    {
        while (!metronomeQueue->empty()) {
            MetronomeEvent event = metronomeQueue->back();
            metronomeQueue->pop_back();
            pulseHeight = 1.0f;
        }

        while (!midiNoteQueue->empty()) {
            MidiNoteEvent event = midiNoteQueue->back();
            midiNoteQueue->pop_back();
            std::cout << "note: " << event.note << " velocity: " << event.velocity << std::endl;
            if (event.note == 36 && event.velocity > 0) {
                pulseHeight = (float)event.velocity / 127.0f;
            }
            cityShader.setInt("selected", event.note);
        }

        while (!envelopeQueue->empty()) {
            EnvelopeEvent event = envelopeQueue->back();
            envelopeQueue->pop_back();
            pulseHeight = event.value;
        }

        pulseHeight *= 0.95f;//60.0f * deltaTime;

        float rotationAngle = 10.0f * t;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;

        scenePredraw();

        geometryShader.use();

        geometryShader.setFloat("t", t);
        geometryShader.setFloat("f", 10.0f);
        geometryShader.setFloat("pulseHeight", pulseHeight);

        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);
        geometryShader.setMat4("model", model);

        plane.draw();

        // if ((int)t % 2 == 0)
        // plane.drawLines();

        // plane.drawPoints();

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0,0.0, 0.0));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0, 10.0, 10.0));
        lineShader.use();
        lineShader.setFloat("t", t);
        lineShader.setFloat("pulseHeight", pulseHeight);
        // lineShader.setMat4("MVP", projection * view * model);
        lineShader.setMat4("projection", projection);
        lineShader.setMat4("view", view);
        lineShader.setMat4("model", model);

        // lineShader.use();
        // bunnyModel.Draw(lineShader);
        // geometryShader.use();
        // bunnyModel.Draw(geometryShader);
        // modelShader.use();
        // bunnyModel.Draw(modelShader);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0,0.0, 0.0));
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(10.0, 10.0, 10.0));

        Mesh mesh = bunnyModel.meshes[0];
        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);

        // "normal" mode

        bunnyShader.use();
        bunnyShader.setFloat("t", t);
        bunnyShader.setMat4("projection", projection);
        bunnyShader.setMat4("view", view);
        bunnyShader.setMat4("model", model);
        bunnyModel.Draw(bunnyShader);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0,0.0, 0.0));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f));
        bunnyShader.setMat4("model", model);
        mesh = sphereModel.meshes[0];
        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

        // lines only
        bunnyLineShader.use();
        bunnyLineShader.setMat4("projection", projection);
        bunnyLineShader.setMat4("view", view);
        bunnyLineShader.setMat4("model", model);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

        // draw city w/ instanced shader
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));

        cityShader.use();
        cityShader.setFloat("t", t);
        cityShader.setFloat("cameraX", camera.Position.x);
        cityShader.setFloat("cameraZ", camera.Position.z);
        cityShader.setFloat("pulseHeight", pulseHeight);

        cityShader.setMat4("projection", projection);
        cityShader.setMat4("view", view);
        cityShader.setMat4("model", model);

        Mesh cityMesh = cityModel.meshes[0];
        glBindVertexArray(cityMesh.VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cityMesh.EBO);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // glDrawElements(GL_TRIANGLES, cityMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
        glDrawElementsInstanced(GL_TRIANGLES, cityMesh.indices.size(), GL_UNSIGNED_INT, (void*)0, 600);

        scenePostdraw();
    }

    glfwTerminate();
    return 0;
}
