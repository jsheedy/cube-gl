#include <iostream>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "utils.hpp"

enum ShaderStyle {
    WIREFRAME,
    FULL
};

int width = 1400;
int height = 900;

float t = 0;
float timeMultiplier = 1.0;
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastX;
float lastY;

bool clearScreen = true;
ShaderStyle shaderStyle = FULL;
bool keyDown = false;


float ROT_SLERP_MIX = 2.0f;

GLFWwindow* window;

Camera camera = Camera(
    glm::vec3(0.0f, 1.0f, 10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    -90.0f,
    -20.0f
);

bool mDown = false;


void processInput(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        camera.Action = CENTER_ROT;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        camera.Action = CENTER_HOVER;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        camera.Action = HOVER_BUNNY;
    }

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        camera.Action = FREELOOK;
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        shaderStyle = WIREFRAME;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        shaderStyle = FULL;
    }

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        clearScreen = !clearScreen;
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        camera.Action = FREELOOK;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS){
        camera.MovementSpeed *= 1.2;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        camera.MovementSpeed /= 1.2;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void scenePredraw() {
    t = glfwGetTime() * timeMultiplier;
    deltaTime = t - lastFrame;
    lastFrame = t;
    if (clearScreen) {
        float r = 0.0f; float g = 0.0f; float b = 0.0f;
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    processInput(window, deltaTime);

    if (camera.Action == CENTER_ROT) {

        glm::vec3 TargetPosition(0.0f, 0.2f, 0.0f);
        glm::vec3 RightVector(1.0f, 0.0f, 0.0f);

        glm::quat rotAround = glm::angleAxis(glm::radians(5.0f * t), UpVector);
        glm::quat rotDown = glm::angleAxis(glm::radians(35.0f), RightVector);
        glm::quat rot = rotDown * rotAround * glm::quat();
        camera.Position = glm::mix(camera.Position, TargetPosition, ROT_SLERP_MIX * deltaTime);
        camera.Orientation = glm::mix(camera.Orientation, rot, ROT_SLERP_MIX * deltaTime);
    }
    else if (camera.Action == CENTER_HOVER) {
        glm::vec3 TargetPosition(0.0f, 3.0f, 0.0f);
        glm::quat rot = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        camera.Position = glm::mix(camera.Position, TargetPosition, ROT_SLERP_MIX * deltaTime);
        camera.Orientation = glm::mix(camera.Orientation, rot, ROT_SLERP_MIX * deltaTime);
    }
    else if (camera.Action == HOVER_BUNNY) {
        glm::vec3 TargetPosition(0.0f, 1.0f, 5.0f);
        glm::quat rot = glm::angleAxis(glm::radians(10.0f), glm::vec3(1.00f, 0.0f, 0.0f));
        camera.Position = glm::mix(camera.Position, TargetPosition, ROT_SLERP_MIX * deltaTime);
        camera.Orientation = glm::mix(camera.Orientation, rot, ROT_SLERP_MIX * deltaTime);
    }
    else if (camera.Action == FREELOOK) {
        //  ¯\_(ツ)_/¯
    }
}

void scenePostdraw(GLFWwindow* window) {
    // double buffering
    // glfwSwapBuffers(window);
    // single buffering;
    glFlush();

    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int _width, int _height)
{
    width = _width;
    height = _height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (camera.Action != FREELOOK) {
        return;
    }
    if (std::isnan(lastX) || std::isnan(lastY))
    {
        // lastX = width / 2.0f;
        // lastY = height / 2.0f;
        lastX = xpos;
        lastY = ypos;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

GLFWwindow* sceneInit(int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    // glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif
    window = glfwCreateWindow(width, height, "MEGACUBE", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return window;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return window;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // backface culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    return window;
}