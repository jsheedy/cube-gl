// std stuff
#include <cmath>
#include <iostream>
#include <vector>

// OSC stuff
#include <atomic>
#include <lo/lo.h>
#include <lo/lo_cpp.h>

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

using std::cout;
using std::endl;
using std::vector;

#define PORT 37341

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

float scale = 1.0f;

Camera camera = Camera(
    glm::vec3(0.0f, 3.0f, 0.01f),
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

    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    unsigned int VBO;
    unsigned int cubeVAO;
    unsigned int N = 700;
    unsigned int M = 700;
    unsigned int sizePoints = N*M*3;
    unsigned int sizeIndices = sizePoints * 2 - M - N;

    float *points = new float[sizePoints];
    unsigned int *pointIndices = new unsigned int[sizeIndices];

    unsigned long idx=0;
    unsigned long pointIdx=0;

    for (int i=0; i<N; i++) {
        for (int j=0; j<M; j++) {
            // x-z plane
            points[idx++] = (((float)i)/N - 0.5f);
            points[idx++] = 0;
            points[idx++] = (((float)j)/M - 0.5f);

            // indexes
            if (i < (N-1) && j < (M-1)) {

                // triangle 1
                pointIndices[pointIdx++] = (i*M + j);
                pointIndices[pointIdx++] = (i*M + j + 1);
                pointIndices[pointIdx++] = ((i+1)*M + j);

                // triangle 2
                pointIndices[pointIdx++] = (i*M + j + 1);
                pointIndices[pointIdx++] = ((i+1)*M + j + 1);
                pointIndices[pointIdx++] = ((i+1)*M + j);
            }
        }
    }

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizePoints * sizeof(float), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * sizeIndices, pointIndices, GL_STATIC_DRAW);

    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)(3* sizeof(float)));
    // glEnableVertexAttribArray(1);

    Shader geometryShader("shaders/geometry.vs", "shaders/geometry.fs", "shaders/geometry.gs");
    // Shader geometryShader("shaders/geometry.vs", "shaders/geometry.fs", "shaders/passthru.gs");

    float t = 0;

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
    camera.MovementSpeed = 1.2f;

    std::vector<int> oscQueue;

    // OSC megahack
    lo::ServerThread st(PORT);
    if (!st.is_valid()) {
        std::cout << "couldn't start OSC server" << std::endl;
        return 1;
    }

    // set up lambda callbacks
    st.set_callbacks([&st]() { printf("OSC THREAD INIT: %p\n", &st);},
        []() {printf("OSC THREAD CLEANUP\n");});

    std::cout << "URL: " << st.url() << std::endl;

    // number of msgs received. use atomic
    std::atomic<int> received(0);

    st.add_method("/midi/note", "iii",
        [&received](lo_arg **argv, int)
        {std::cout << "/midi/note (" << (++received) << "): "
            << " note: " << argv[0]->i
            << " velocity: " << argv[1]->i
            << " channel: " << argv[2]->i
            << std::endl;});

    st.add_method("/metronome", "ii",
        [&received, &oscQueue](lo_arg **argv, int)
        {
            int bpm = argv[0]->i;
            std::cout << "/metronome (" << (++received) << "): "
            << " bpm: " << bpm
            << " beat: " << argv[1]->i
            << std::endl;
            oscQueue.push_back(bpm);
        });

    st.start();
    // OSC megahack

    float pulseHeight = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        if (!oscQueue.empty()) {
            int rv = oscQueue.back();
            oscQueue.pop_back();
            std::cout << "OSC: " << rv << std::endl;
            pulseHeight = 1.0f;
        }
        t = glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        pulseHeight *= 0.95f; // FIXME: use times Time.deltaTime
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
        geometryShader.setFloat("pulseHeight", pulseHeight);
        float rotationAngle = 1.0f * t;

        glm::vec3 cubePosition = glm::vec3(0.0f, 0.0f,  0.0f);
        glm::mat4 model;
        model = glm::translate(model, cubePosition);
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));
        geometryShader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeIndices, GL_UNSIGNED_INT, (void*)0);
        // glDrawElements(GL_LINES, sizeIndices, GL_UNSIGNED_INT, (void*)0);

        // double buffering
        // glfwSwapBuffers(window);
        // single buffering;
        glFlush();

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
