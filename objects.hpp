#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

        sizePoints = N*M*3;
        sizeIndices = sizePoints * 2;// - M - N;

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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * sizeIndices, pointIndices, GL_STATIC_DRAW);

        // grid
        glGenBuffers(1, &GridEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GridEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * sizeIndices, gridIndices, GL_STATIC_DRAW);
    }

    void draw() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glDrawElements(GL_TRIANGLES, sizeIndices, GL_UNSIGNED_INT, (void*)0);
    }

    void drawPoints() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glDrawElements(GL_POINTS, sizeIndices, GL_UNSIGNED_INT, (void*)0);
    }

    void drawLines() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->GridEBO);
        glDrawElements(GL_LINES, sizeIndices, GL_UNSIGNED_INT, (void*)0);
    }
};