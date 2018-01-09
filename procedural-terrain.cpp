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


// unsigned int N = 128;
// unsigned int M = 128;

// unsigned int N = 256;
// unsigned int M = 256;

unsigned int N = 512;
unsigned int M = 512;

// unsigned int N = 1024;
// unsigned int M = 1024;

// unsigned int N = 2048;
// unsigned int M = 2048;

int main()
{
    GLFWwindow* window = sceneInit(width, height);

    camera.Position = glm::vec3(0.0f, 1.5f, 2.0f);
    camera.MovementSpeed = 2.0f;
    camera.MouseSensitivity = 0.002f;
    camera.LookAt(glm::vec3(0.0, -1.0, 0.0));

    OSCServer oscServer(37341);
    // oscServer.start();


    Cube cube;

    // Shader terrainShader("shaders/vertex/geometry.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    Shader terrainLineShader("shaders/vertex/height-map.vs", "shaders/fragment/geometry-texture.fs", "shaders/geometry/geometry.gs");
    // Shader terrainLineShader("shaders/vertex/height-map.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    // Shader cubeShader("shaders/vertex/passthru.vs", "shaders/fragment/lines.fs", NULL);
    Shader cubeShader("shaders/vertex/wood-cube.vs", "shaders/fragment/texture.fs", NULL);

    unsigned int pavementTexture = loadTexture("assets/pavement.jpg", GL_RGBA);

    unsigned int heightMapTexture_12_03 = loadTexture("assets/srtm/srtm_12_03-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png
    unsigned int heightMapTexture_12_04 = loadTexture("assets/srtm/srtm_12_04-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png
    unsigned int heightMapTexture_13_03 = loadTexture("assets/srtm/srtm_13_03-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png
    unsigned int heightMapTexture_13_04 = loadTexture("assets/srtm/srtm_13_04-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png

    // unsigned int heightMapTexture = loadTexture("assets/iceland_terrain_map.png", GL_RED);
    unsigned int uvTestTexture = loadTexture("assets/01-uv-texture.png", GL_RGB);

    terrainLineShader.use();
    terrainLineShader.setInt("texture1", 0);
    terrainLineShader.setInt("heightMap", 1);

    Plane plane = Plane(N, M);
    Axes axes = Axes();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100000.0f);

    bool keyDown = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(!glfwWindowShouldClose(window))
    {
        scenePredraw();

        glm::mat4 view = camera.GetViewMatrix();

        cubeShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pavementTexture);

        glm::mat4 model;
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(t * 90.0f), ForwardVector);
        cube.drawLines(cubeShader, model, view, projection);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(t * 90.0f), UpVector);
        cube.drawLines(cubeShader, model, view, projection);

        terrainLineShader.use();
        terrainLineShader.setFloat("t", t);
        terrainLineShader.setVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        // draw planes x4

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_12_03);
        // glBindTexture(GL_TEXTURE_2D, uvTestTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_12_03);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        plane.draw(terrainLineShader, model, view, projection);

        // plane 2
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_13_03);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_13_03);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        plane.draw(terrainLineShader, model, view, projection);

        // plane 3
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_12_04);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_12_04);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        plane.draw(terrainLineShader, model, view, projection);

        // plane 4
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_13_04);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, heightMapTexture_13_04);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        plane.draw(terrainLineShader, model, view, projection);

        scenePostdraw();

        // assert(glGetError() == GL_NO_ERROR);
    }

    glfwTerminate();
    return 0;
}
