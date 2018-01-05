// std stuff
#include <cmath>
#include <iostream>
#include <sstream>

// GL stuff
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// local stuff
#include "shader.h"
#include "osc.hpp"
#include "model.hpp"
#include "objects.hpp"
#include "scene.hpp"


unsigned int N = 100;
unsigned int M = 100;

void Draw(Model model, Shader shader) {

}

int main()
{
    GLFWwindow* window = sceneInit(width, height);

    camera.Position = glm::vec3(0.0f, 100.0f, 1000.0f);
    camera.MovementSpeed = 200.0f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    OSCServer oscServer(37341);
    // oscServer.start();

    glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);

    // reverse the order of these to get some geometry glitch  ¯\_(ツ)_/¯
    Shader geometryShader("shaders/vertex/geometry.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    Shader bunnyShader("shaders/vertex/passthru.vs", "shaders/fragment/bunny.fs", "shaders/geometry/geometry.gs");
    Shader sphereShader("shaders/vertex/passthru.vs", "shaders/fragment/sphere.fs",  "shaders/geometry/geometry.gs");
    Shader cityShader("shaders/vertex/instanced.vs", "shaders/fragment/city.fs", NULL);
    Shader cityLineShader("shaders/vertex/instanced.vs", "shaders/fragment/lines.fs", NULL);
    Shader bunnyLineShader("shaders/vertex/MVP.vs", "shaders/fragment/lines.fs", NULL);
    Shader sphereLineShader("shaders/vertex/MVP.vs", "shaders/fragment/lines.fs", NULL);
    Shader lineShader("shaders/vertex/lines.vs", "shaders/fragment/lines-blue.fs", "shaders/geometry/lines-wide.gs");
    Plane plane = Plane(N, M);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000000.0f);
    float pulseHeight = 0.0f;

    std::vector<MidiNoteEvent> *midiNoteQueue = &oscServer.midiNoteQueue[1];
    std::vector<MetronomeEvent> *metronomeQueue = &oscServer.metronomeQueue;
    std::vector<EnvelopeEvent> *envelopeQueue = &oscServer.envelopeQueue[1];

    Model nanoModel((char *)"assets/nanosuit/nanosuit.obj");
    Model velotronModel((char *)"assets/velotron_arise_eecc.fbx");
    Model cityModel((char *)"assets/SciFi_HumanCity_Kit05-OBJ.obj");
    Model bunnyModel((char *)"assets/bunny.ply");
    Model sphereModel((char *)"assets/sphere.ply");

    Cube cube;

    Mesh bunnyMesh = bunnyModel.meshes[0];
    Mesh cityMesh = cityModel.meshes[0];
    Mesh sphereMesh = sphereModel.meshes[0];

    glm::vec3 LookTarget;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(!glfwWindowShouldClose(window))
    {
        scenePredraw(camera);

        while (!metronomeQueue->empty()) {
            MetronomeEvent event = metronomeQueue->back();
            metronomeQueue->pop_back();
            pulseHeight = 1.0f;
        }

        while (!midiNoteQueue->empty()) {
            MidiNoteEvent event = midiNoteQueue->back();
            midiNoteQueue->pop_back();
            if (event.note == 36 && event.velocity > 0) {
                pulseHeight = (float)event.velocity / 127.0f;
            }
            if (event.velocity == 0) {
                // cityShader.setInt("selected", 0);
            } else {
                cityShader.use();
                cityShader.setInt("selected", event.note);
                std::cout << event.note << std::endl;
            }
        }

        while (!envelopeQueue->empty()) {
            EnvelopeEvent event = envelopeQueue->back();
            envelopeQueue->pop_back();
            pulseHeight = event.value;
        }

        pulseHeight *= 0.95f;//60.0f * deltaTime;
        glm::mat4 model;

        glm::mat4 view(camera.Orientation);
        view = glm::translate(view, -camera.Position);

        lineShader.use();
        lineShader.setFloat("t", t);
        lineShader.setFloat("pulseHeight", pulseHeight);
        lineShader.setMat4("projection", projection);
        lineShader.setMat4("view", view);
        lineShader.setMat4("model", model);

        glm::mat4 bunnyModelMatrix;
        bunnyModelMatrix = glm::translate(bunnyModelMatrix, glm::vec3(0.0, -35.0, 0.0));
        bunnyModelMatrix = glm::scale(bunnyModelMatrix, glm::vec3(1000.0f));

        glm::mat4 cityModelMatrix;
        // cityModelMatrix = glm::scale(cityModelMatrix, glm::vec3(1.0f));

        glm::mat4 sphereModelMatrix;
        sphereModelMatrix = glm::scale(sphereModelMatrix, glm::vec3(0.5f));
        sphereModelMatrix = glm::rotate(sphereModelMatrix, glm::radians(t * 100.0f), glm::vec3(0.0, 1.0, 0.0));

        cityShader.use();
        cityShader.setFloat("t", t);
        cityShader.setFloat("cameraX", camera.Position.x);
        cityShader.setFloat("cameraZ", camera.Position.z);
        cityShader.setFloat("pulseHeight", pulseHeight);
        cityShader.setMat4("MVP", projection * view * cityModelMatrix);
        cityShader.setMat4("model", cityModelMatrix);
        cityShader.setMat4("view", view);

        cityLineShader.use();
        cityLineShader.setFloat("t", t);
        cityLineShader.setFloat("cameraX", camera.Position.x);
        cityLineShader.setVec4("lineColor", blue);
        cityLineShader.setFloat("cameraZ", camera.Position.z);
        cityLineShader.setFloat("pulseHeight", pulseHeight);
        cityLineShader.setMat4("MVP", projection * view * cityModelMatrix);

        bunnyShader.use();
        bunnyShader.setFloat("t", t);
        bunnyShader.setFloat("alpha", (glm::sin(t)/4.0 + 0.75f));
        bunnyShader.setMat4("projection", projection);
        bunnyShader.setMat4("view", view);
        bunnyShader.setMat4("model", bunnyModelMatrix);

        bunnyLineShader.use();
        bunnyLineShader.setFloat("t", t);
        bunnyLineShader.setVec4("lineColor", green);
        bunnyLineShader.setMat4("projection", projection);
        bunnyLineShader.setMat4("view", view);
        bunnyLineShader.setMat4("model", bunnyModelMatrix);

        sphereShader.use();
        sphereShader.setFloat("t", t);
        sphereShader.setMat4("projection", projection);
        sphereShader.setMat4("view", view);
        sphereShader.setMat4("model", sphereModelMatrix);
        sphereShader.setMat4("MVP", projection * view * sphereModelMatrix);

        sphereLineShader.use();
        sphereLineShader.setFloat("t", t);
        sphereLineShader.setVec4("lineColor", red);
        sphereLineShader.setMat4("projection", projection);
        sphereLineShader.setMat4("view", view);
        sphereLineShader.setMat4("model", sphereModelMatrix);
        sphereLineShader.setMat4("MVP", projection * view * sphereModelMatrix);

        if (shaderStyle == FULL) {

            cityShader.use();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBindVertexArray(cityMesh.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cityMesh.EBO);
            glDrawElementsInstanced(GL_TRIANGLES, cityMesh.indices.size(), GL_UNSIGNED_INT, (void*)0, 100);

            bunnyShader.use();
            glBindVertexArray(bunnyMesh.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyMesh.EBO);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, bunnyMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

            glBindVertexArray(sphereMesh.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereMesh.EBO);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, sphereMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

        } else {

            cityLineShader.use();
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindVertexArray(cityMesh.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cityMesh.EBO);
            glDrawElementsInstanced(GL_TRIANGLES, cityMesh.indices.size(), GL_UNSIGNED_INT, (void*)0, 100);

            // bunny
            bunnyLineShader.use();
            glBindVertexArray(bunnyMesh.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyMesh.EBO);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, bunnyMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

            sphereLineShader.use();
            glBindVertexArray(sphereMesh.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereMesh.EBO);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, sphereMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

            cube.drawLines(bunnyLineShader, glm::mat4(), view, projection);
        }

        scenePostdraw();
    }

    glfwTerminate();
    return 0;
}
