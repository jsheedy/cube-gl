#include <iostream>
#include <cmath>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.h"
#include "camera.h"
#include "textures.h"
#include "cube_vertices.h"

using std::cout;
using std::endl;
using std::vector;

int width = 1400;
int height = 900;

// int width = 1920;
// int height = 1080;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool clearScreen = true;
float lastX;
float lastY;

Camera camera = Camera(
    glm::vec3(0.0f, 2.0f, 5.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    -90.0f,
    -20.0f
);

void framebuffer_size_callback(GLFWwindow* window, int _width, int _height)
{
    width = _width;
    height = _height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        clearScreen = !clearScreen;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (std::isnan(lastX) || std::isnan(lastY))
    {
        lastX = xpos;
        lastY = ypos;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void drawLights(glm::mat4 projection, glm::mat4 view, Shader shader, unsigned int VAO, vector<glm::vec3> lightPositions)
{
    shader.use();
    glBindVertexArray(VAO);

    int modelLoc = glGetUniformLocation(shader.ID, "model");
    int viewLoc = glGetUniformLocation(shader.ID, "view");
    int projectionLoc = glGetUniformLocation(shader.ID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    shader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

    for(vector<glm::vec3>::const_iterator iter = lightPositions.begin();
        iter != lightPositions.end();
        ++iter) {
        glm::mat4 model;
        model = glm::translate(model, *iter);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

GLFWwindow* init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif

        GLFWwindow* window = glfwCreateWindow(width, height, "MEGACUBE", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return window;
    }
    glfwMakeContextCurrent(window);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return window;
    }
    return window;
}

int main()
{
    GLFWwindow* window = init();

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int cubeVAO;
    unsigned int lightVAO;

    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = loadTexture("assets/container2.png", GL_RGBA);
    unsigned int specularMap = loadTexture("assets/container2_specular.png", GL_RGBA);
    unsigned int emissionMap = loadTexture("assets/matrix.jpg", GL_RGB);

    Shader lightingShader("shaders/brain-glow.vs", "shaders/brain-glow.fs");
    Shader lampShader("shaders/lamp.vs", "shaders/lamp.fs");

    // don't forget to 'use' the corresponding shader program first (to set the uniform)
    lightingShader.use();
    lightingShader.setVec3("objectColor", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("lightColor",  1.0f, 0.0f, 0.0f);

    lightingShader.setVec3("material.ambient",  1.0f, 0.5f, 0.2f);
    lightingShader.setFloat("material.shininess", 64.0f);

    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.emission", 2);

    vector<glm::vec3> pointLightPositions;
    pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, 2.0f));
    pointLightPositions.push_back(glm::vec3(2.0f, 0.0f, 0.0f));
    pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, -2.0f));
    pointLightPositions.push_back(glm::vec3(-2.0f, 0.0f, 0.0f));

    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    for (int i=0; i < pointLightPositions.size(); i++) {
        std::string id = "pointLights[" + std::to_string(i) + "]";
        lightingShader.setFloat(id + ".constant", 1.0f);
        lightingShader.setFloat(id + ".linear", 0.09f);
        lightingShader.setFloat(id + ".quadratic", 0.032f);
        lightingShader.setVec3(id + ".ambient", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3(id + ".diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3(id + ".specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3(id + ".position", pointLightPositions[i]);
    }

    float t = 0;
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window, mouse_callback);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);

    // set up floating point framebuffer to render scene to
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    // FIXME: bind this in order to get MRT (Multiple Render Target)
    // glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
    }
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // output render buffers

    while(!glfwWindowShouldClose(window))
    {
        t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        processInput(window);

        // float LFO = (sin(currentFrame*1.10f) / 2.0f) + 0.5f;
        if (clearScreen) {
            float r = 0.0f; float g = 0.0f; float b = 0.0f;
            glClearColor(r, g, b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }


        glm::mat4 view = camera.GetViewMatrix();
        drawLights(projection, view, lampShader, lightVAO, pointLightPositions);

        // draw cube
        lightingShader.use();
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);
        lightingShader.setFloat("t", t);
        float rotationAngle = 0.8f * t;

        glBindVertexArray(cubeVAO);

        for (float x=-10.0f; x < 11.0f; x += 2.0f) {
            for (float z=-10.0f; z < 11.0f; z += 2.0f) {
                glLineWidth(100.0f);
                glm::vec3 cubePosition = glm::vec3(x, 0.0f,  z);
                glm::mat4 model;
                model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, cubePosition);
                model = glm::scale(model, glm::vec3(0.8f));
                lightingShader.setMat4("model", model);
                // glDrawArrays(GL_TRIANGLES, 0, 36);
                glDrawArrays(GL_LINE_STRIP, 0, 36);
                // glDrawArrays(GL_LINES, 0, 36);
                // glDrawArrays(GL_LINE_LOOP, 0, 36);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
