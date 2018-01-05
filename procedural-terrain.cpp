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
#include "utils.hpp"


unsigned int N = 100;
unsigned int M = 100;

int main()
{
    GLFWwindow* window = sceneInit(width, height);

    camera.Position = glm::vec3(0.0f, 0.0f, 10.0f);
    camera.MovementSpeed = 200.0f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    OSCServer oscServer(37341);
    // oscServer.start();

    Cube cube;

    Shader terrainShader("shaders/vertex/geometry.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    Shader terrainLineShader("shaders/vertex/MVP.vs", "shaders/fragment/lines.fs", NULL);
    // Shader cubeShader("shaders/vertex/passthru.vs", "shaders/fragment/lines.fs", NULL);
    Shader cubeShader("shaders/vertex/wood-cube.vs", "shaders/fragment/texture.fs", NULL);

    unsigned int texture = loadTexture("assets/container2.png", GL_RGBA);

    cubeShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    Plane plane = Plane(N, M);
    Axes axes = Axes();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100000.0f);

    bool keyDown = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    cubeShader.use();
    cubeShader.setVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    while(!glfwWindowShouldClose(window))
    {
        scenePredraw(camera);

        glm::mat4 view(camera.Orientation);
        view = glm::translate(view, -camera.Position);


        // glm::mat4 terrainModelMatrix;
        // terrainModelMatrix = glm::scale(terrainModelMatrix, glm::vec3(1.0f));
        // terrainModelMatrix = glm::rotate(terrainModelMatrix, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        // terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(0.0, 0.0, 0.0));

        // glm::mat4 view(camera.Orientation);
        // view = glm::translate(view, -camera.Position);

        // terrainLineShader.use();
        // terrainLineShader.setMat4("projection", projection);
        // terrainLineShader.setMat4("view", view);
        // terrainLineShader.setMat4("model", terrainModelMatrix);
        // terrainLineShader.setVec3("lineColor", green);

        // terrainShader.use();
        // terrainShader.setMat4("projection", projection);
        // terrainShader.setMat4("view", view);
        // terrainShader.setMat4("model", terrainModelMatrix);

        // if (shaderStyle == FULL) {

        //     terrainShader.use();
        //     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //     glBindVertexArray(plane.VAO);
        //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.EBO);
        //     glDrawElements(GL_TRIANGLES, plane.sizeIndices, GL_UNSIGNED_INT, (void*)0);

        // } else {

        //     terrainLineShader.use();
        //     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //     glBindVertexArray(plane.VAO);
        //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.GridEBO);
        //     glDrawElements(GL_LINES, plane.sizeIndices, GL_UNSIGNED_INT, (void*)0);
        // }

        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
        cube.drawLines(cubeShader, model, view, projection);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
        cube.drawLines(cubeShader, model, view, projection);
        scenePostdraw();
    }

    glfwTerminate();
    return 0;
}
