// std stuff
#include <cmath>
#include <iostream>
#include <vector>

// GL stuff
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// local stuff
#include "shader.h"
#include "camera.h"
#include "textures.h"
#include "cube_vertices.h"
#include "osc.hpp"
#include "objects.hpp"

using std::cout;
using std::endl;
using std::vector;

unsigned int N = 100;
unsigned int M = 100;
int width = 1400;
int height = 900;

// int width = 1920;
// int height = 1080;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool clearScreen = true;
bool mouseLookOn = false;
float lastX;
float lastY;

float scale = 100.0f;

Camera camera = Camera(
    glm::vec3(0.0f, 100.0f, 0.1f),
    // glm::vec3(0.0f, 0.4f, 1.1f),
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

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        mouseLookOn = !mouseLookOn;

    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
        camera.MovementSpeed *= 1.2;

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
        camera.MovementSpeed /= 1.2;

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
    if (!mouseLookOn) {
        return;
    }
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

GLFWwindow* init(int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
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

    glEnable(GL_DEPTH_TEST);
    glfwSetCursorPosCallback(window, mouse_callback);

    return window;
}

int main()
{
    GLFWwindow* window = init(width, height);
    OSCServer oscServer(37341);
    oscServer.start();

    // reverse the order of these to get some geometry glitch  ¯\_(ツ)_/¯
    Shader geometryShader("shaders/geometry.vs", "shaders/geometry.fs", "shaders/geometry.gs");
    Plane plane = Plane(N, M);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
    camera.MovementSpeed = 1.2f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    float t = 0;
    float pulseHeight = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        if (!oscServer.metronomeQueue.empty()) {
            int rv = oscServer.metronomeQueue.back();
            oscServer.metronomeQueue.pop_back();
            std::cout << "bpm: " << rv << std::endl;
            pulseHeight = 1.0f;
        }

        t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        pulseHeight *= 0.95f;//60.0f * deltaTime;
        processInput(window);

        if (clearScreen) {
            float r = 0.0f; float g = 0.0f; float b = 0.0f;
            glClearColor(r, g, b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        glm::mat4 view = camera.GetViewMatrix();

        // draw cube
        geometryShader.use();
        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);
        geometryShader.setFloat("t", t);
        geometryShader.setFloat("f", 10.0f);
        geometryShader.setFloat("pulseHeight", pulseHeight);
        float rotationAngle = 1.0f * t;

        glm::vec3 pos = glm::vec3(0.0f, 0.0f,  0.0f);
        glm::mat4 model;
        model = glm::translate(model, pos);
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        geometryShader.setMat4("model", model);

        plane.draw();

        // double buffering
        // glfwSwapBuffers(window);
        // single buffering;
        glFlush();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
