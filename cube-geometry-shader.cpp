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
    glm::vec3(0.0f, 1.0f, 2.0f),
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

GLFWwindow* init(int width, int height)
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

    glEnable(GL_DEPTH_TEST);
    glfwSetCursorPosCallback(window, mouse_callback);

    return window;
}

int main()
{
    GLFWwindow* window = init(width, height);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int cubeVAO;
    // unsigned int lightVAO;

    glGenVertexArrays(1, &cubeVAO);
    // glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    // Shader lightingShader("shaders/brain-glow.vs", "shaders/brain-glow.fs");

    Shader geometryShader("shaders/geometry.vs", "shaders/geometry.fs", "shaders/geometry.gs");

    float t = 0;

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
    camera.MovementSpeed = 15.0f;

    while(!glfwWindowShouldClose(window))
    {
        t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        processInput(window);

        if (clearScreen) {
            float r = 0.0f; float g = 0.0f; float b = 0.0f;
            glClearColor(r, g, b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }


        glm::mat4 view = camera.GetViewMatrix();

        // draw cube
        geometryShader.use();
        geometryShader.setMat4("view", view);
        geometryShader.setMat4("projection", projection);
        // geometryShader.setFloat("t", t);
        float rotationAngle = 10.4f * t;

        glBindVertexArray(cubeVAO);

        glm::vec3 cubePosition = glm::vec3(0.0f, 0.0f,  0.0f);
        glm::mat4 model;
        model = glm::translate(model, cubePosition);
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f));
        geometryShader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glDrawArrays(GL_LINES, 0, 36);
        // glDrawArrays(GL_LINES_ADJACENCY, 0, 36);
        glDrawArrays(GL_POINTS, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
