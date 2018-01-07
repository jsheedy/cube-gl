#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Geometry {};

class Plane: Geometry {
    private:

    public:
        unsigned int sizeIndices;
        unsigned int sizePoints;

        unsigned int VBO;
        unsigned int EBO;
        unsigned int VAO;
        unsigned int GridEBO;

        glm::vec3 Position = glm::vec3(0.0f, 0.0f,  0.0f);


    Plane(int N, int M) {

        sizePoints = N*M*3*2;
        sizeIndices = sizePoints;// - M - N;

        float *points = new float[sizePoints];
        unsigned int *pointIndices = new unsigned int[sizeIndices];
        unsigned int *gridIndices = new unsigned int[sizeIndices];

        unsigned long idx=0;
        unsigned long pointIdx=0;
        unsigned long gridIdx=0;

        for (int i=0; i<N; i++) {
            for (int j=0; j<M; j++) {
                // x-z plane
                points[idx++] = (((float)i)/N - 0.5f);
                points[idx++] = 0;
                points[idx++] = (((float)j)/M - 0.5f);
                // UV
                points[idx++] = ((float)i)/N;
                points[idx++] = ((float)j)/M;

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

                    // rectangular grid

                    gridIndices[gridIdx++] = (i*M + j);
                    gridIndices[gridIdx++] = (i*M + j + 1);

                    gridIndices[gridIdx++] = (i*M + j);
                    gridIndices[gridIdx++] = ((i+1)*M + j);
                } else if (j == (M-1) && i < (N-1)) {
                    // rectangular grid
                    gridIndices[gridIdx++] = (i*M + j);
                    gridIndices[gridIdx++] = ((i+1)*M + j);

                } else if (i == (N-1) && j < (M-1)) {
                    gridIndices[gridIdx++] = (i*M + j);
                    gridIndices[gridIdx++] = (i*M + j + 1);
                }
            }
        }

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizePoints * sizeof(float), points, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * sizeIndices, pointIndices, GL_STATIC_DRAW);

        // grid
        glGenBuffers(1, &GridEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GridEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * sizeIndices, gridIndices, GL_STATIC_DRAW);
    }

    void draw(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeIndices, GL_UNSIGNED_INT, (void*)0);
    }

    void drawPoints() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_POINTS, sizeIndices, GL_UNSIGNED_INT, (void*)0);
    }

    void drawLines(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GridEBO);
        glDrawElements(GL_LINES, sizeIndices, GL_UNSIGNED_INT, (void*)0);
    }
};


class Axes: Geometry {

    public:
        unsigned int sizeIndices;
        unsigned int sizePoints;

        unsigned int VBO;
        unsigned int EBO;
        unsigned int VAO;

        glm::vec3 Position = glm::vec3(0.0f, 0.0f,  0.0f);

        Shader* shader;

    Axes() {

        shader = new Shader("shaders/vertex/MVP.vs", "shaders/fragment/lines.fs", NULL);

        float *points = new float[12];
        unsigned int *pointIndices = new unsigned int[6];

        unsigned long idx=0;
        unsigned long pointIdx=0;
        unsigned long gridIdx=0;

        points[idx++] = 0.0f;
        points[idx++] = 0.0f;
        points[idx++] = 0.0f;

        points[idx++] = 1.0f;
        points[idx++] = 0.0f;
        points[idx++] = 0.0f;

        points[idx++] = 0.0f;
        points[idx++] = 1.0f;
        points[idx++] = 0.0f;

        points[idx++] = 0.0f;
        points[idx++] = 0.0f;
        points[idx++] = 1.0f;

        pointIndices[pointIdx++] = 0;
        pointIndices[pointIdx++] = 1;

        pointIndices[pointIdx++] = 0;
        pointIndices[pointIdx++] = 2;

        pointIndices[pointIdx++] = 0;
        pointIndices[pointIdx++] = 3;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 2, pointIndices, GL_STATIC_DRAW);

    }

    void drawLines(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        shader->use();
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_LINES, 3, GL_UNSIGNED_INT, (void*)0);
    }
};

class Cube : Geometry {
    public:
        unsigned int VBO;
        // unsigned int EBO;
        unsigned int VAO;

    Cube() {

        float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
        };

        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void drawLines(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

};