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


// unsigned int N = 64;
// unsigned int M = 64;

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

// unsigned int N = 4096;
// unsigned int M = 4096;

void drawPlane(Plane plane, Shader shader, glm::vec3 loc, glm::mat4 view, glm::mat4 projection, unsigned int heightTexture, unsigned int texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 model = glm::translate(glm::mat4(), loc);
    plane.draw(shader, model, view, projection);
}

void drawPlaneWireframe(Plane plane, Shader shader, glm::vec3 loc, glm::mat4 view, glm::mat4 projection, unsigned int heightTexture, unsigned int texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 model = glm::translate(glm::mat4(), loc);
    plane.drawLines(shader, model, view, projection);
}

void drawPlaneWireframeGrid(Plane plane, Shader shader, glm::vec3 loc, glm::mat4 view, glm::mat4 projection, unsigned int heightTexture, unsigned int texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 model = glm::translate(glm::mat4(), loc);
    plane.drawGridLines(shader, model, view, projection);
}

int main()
{
    GLFWwindow* window = sceneInit(width, height);

    camera.Position = glm::vec3(0.0f, 1.5f, 2.0f);
    camera.MovementSpeed = 2.0f;
    camera.MouseSensitivity = 0.0005f;
    camera.LookAt(glm::vec3(0.0, -1.0, 0.0));

    OSCServer oscServer(37341);
    // oscServer.start();

    Cube cube;
    Axes axes;

    // Shader terrainShader("shaders/vertex/geometry.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    // Shader terrainShaderHeightMapNormals("shaders/vertex/height-map-normals.vs", "shaders/fragment/geometry-texture.fs", NULL);
    Shader terrainShader("shaders/vertex/height-map.vs", "shaders/fragment/geometry-texture.fs", "shaders/geometry/geometry.gs");
    Shader terrainLineShader("shaders/vertex/height-map-MVP.vs", "shaders/fragment/lines.fs", NULL);
    // Shader terrainShader("shaders/vertex/height-map.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    // Shader cubeShader("shaders/vertex/passthru.vs", "shaders/fragment/lines.fs", NULL);
    Shader cubeShader("shaders/vertex/wood-cube.vs", "shaders/fragment/texture.fs", NULL);

    // unsigned int pavementTexture = loadTexture("assets/ESO_-_Milky_Way.jpg", GL_RGBA);
    unsigned int pavementTexture = loadTexture("assets/pavement.jpg", GL_RGBA);

    unsigned int heightMapTexture_12_03 = loadTexture("assets/srtm/srtm_12_03-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png
    unsigned int heightMapTexture_12_04 = loadTexture("assets/srtm/srtm_12_04-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png
    unsigned int heightMapTexture_13_03 = loadTexture("assets/srtm/srtm_13_03-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png
    unsigned int heightMapTexture_13_04 = loadTexture("assets/srtm/srtm_13_04-2048x2048.png", GL_RED); // gdal_translate -of PNG -ot Byte -scale srtm_12_03.tif srtm_12_03.png  ; convert -scale 2048x2048 assets/srtm_12_03.png assets/srtm_12_03-2048x2048.png

    // unsigned int heightMapTexture = loadTexture("assets/iceland_terrain_map.png", GL_RED);
    // unsigned int uvTestTexture = loadTexture("assets/height-experiment.png", GL_RGBA);
    unsigned int uvTestTexture = loadTexture("assets/01-uv-texture.png", GL_RGB);

    terrainShader.use();
    terrainShader.setInt("texture1", 0);
    terrainShader.setInt("heightMap", 1);

    terrainLineShader.use();
    terrainLineShader.setVec4("lineColor", glm::vec4(0.0, 1.0, 0.0, 1.0));
    terrainLineShader.setInt("heightMap", 1);

    Plane plane = Plane(N, M);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.01f, 100.0f);

    bool keyDown = false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<EnvelopeEvent> *envelopeQueue = &oscServer.envelopeQueue[1];
    std::vector<MetronomeEvent> *metronomeQueue = &oscServer.metronomeQueue;

    float pulseHeight = 1.0f;
    float lightIntensity = 1.0f;
    while(!glfwWindowShouldClose(window))
    {
        while (!envelopeQueue->empty()) {
            EnvelopeEvent event = envelopeQueue->back();
            envelopeQueue->pop_back();
            pulseHeight = event.value;
        }
        while (!metronomeQueue->empty()) {
            MetronomeEvent event = metronomeQueue->back();
            metronomeQueue->pop_back();
            lightIntensity = 1.0f;
        }

        scenePredraw();

        glm::mat4 view = camera.GetViewMatrix();

        cubeShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uvTestTexture);

        glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(t * 30.0f), UpVector);
        model = glm::scale(model, glm::vec3(0.2f));
        cube.drawLines(cubeShader, model, view, projection);

        axes.drawLines(view, projection);

        if (shaderStyle == WIREFRAME_GRID) {
            terrainLineShader.use();
            terrainLineShader.setFloat("t", t);
            terrainLineShader.setFloat("pulseHeight", pulseHeight);
            terrainLineShader.setFloat("lightIntensity", lightIntensity);
            terrainLineShader.setVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            drawPlaneWireframeGrid(plane, terrainLineShader, glm::vec3(0.0f, 0.0f, 0.0f), view, projection, heightMapTexture_12_03, heightMapTexture_12_03);
        }
        else if (shaderStyle == WIREFRAME) {
            terrainLineShader.use();
            terrainLineShader.setFloat("t", t);
            terrainLineShader.setFloat("pulseHeight", pulseHeight);
            terrainLineShader.setFloat("lightIntensity", lightIntensity);
            terrainLineShader.setVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            drawPlaneWireframe(plane, terrainLineShader, glm::vec3(0.0f, 0.0f, 0.0f), view, projection, heightMapTexture_12_03, heightMapTexture_12_03);
        }
        else if (shaderStyle == FULL) {
            terrainShader.use();
            terrainShader.setFloat("t", t);
            terrainShader.setFloat("pulseHeight", pulseHeight);
            terrainShader.setFloat("lightIntensity", lightIntensity);
            terrainShader.setVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            drawPlane(plane, terrainShader, glm::vec3(0.0f, 0.0f, 0.0f), view, projection, heightMapTexture_12_03, heightMapTexture_12_03);
            drawPlane(plane, terrainShader, glm::vec3(1.0f, 0.0f, 0.0f), view, projection, heightMapTexture_13_03, heightMapTexture_13_03);
            drawPlane(plane, terrainShader, glm::vec3(0.0f, 0.0f, 1.0f), view, projection, heightMapTexture_12_04, heightMapTexture_12_04);
            drawPlane(plane, terrainShader, glm::vec3(1.0f, 0.0f, 1.0f), view, projection, heightMapTexture_13_04, heightMapTexture_13_04);
        }

        scenePostdraw(window);
        // lightIntensity *= 0.8f;
    }

    glfwTerminate();
    return 0;
}
