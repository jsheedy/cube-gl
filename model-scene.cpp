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


unsigned int N = 100;
unsigned int M = 100;

float angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 origin) {
    glm::vec3 da=glm::normalize(a-origin);
    glm::vec3 db=glm::normalize(b-origin);
    return glm::acos(glm::dot(da, db));
}

int main()
{
    GLFWwindow* window = sceneInit(width, height);

    camera.Position = glm::vec3(0.0f, 100.0f, 1000.0f);
    camera.MovementSpeed = 200.0f;
    camera.LookAt(glm::vec3(0.0, 0.0, 0.0));

    OSCServer oscServer(37341);
    // oscServer.start();

    // reverse the order of these to get some geometry glitch  ¯\_(ツ)_/¯
    Shader geometryShader("shaders/vertex/geometry.vs", "shaders/fragment/geometry.fs", "shaders/geometry/geometry.gs");
    Shader bunnyShader("shaders/vertex/passthru.vs", "shaders/fragment/bunny.fs", "shaders/geometry/geometry.gs");
    Shader cityShader("shaders/vertex/instanced.vs", "shaders/fragment/city.fs", NULL);
    Shader bunnyLineShader("shaders/vertex/MVP.vs", "shaders/fragment/lines-blue.fs", NULL);
    Shader lineShader("shaders/vertex/lines.vs", "shaders/fragment/lines-blue.fs", "shaders/geometry/lines-wide.gs");
    // Shader lineShader("shaders/lines.vs", "shaders/lines.fs", "shaders/passthru-lines.gs");
    Plane plane = Plane(N, M);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100000.0f);
    float pulseHeight = 0.0f;

    std::vector<MidiNoteEvent> *midiNoteQueue = &oscServer.midiNoteQueue[1];
    std::vector<MetronomeEvent> *metronomeQueue = &oscServer.metronomeQueue;
    std::vector<EnvelopeEvent> *envelopeQueue = &oscServer.envelopeQueue[1];

    Model nanoModel((char *)"assets/nanosuit/nanosuit.obj");
    Model velotronModel((char *)"assets/velotron_arise_eecc.fbx");
    Model cityModel((char *)"assets/SciFi_HumanCity_Kit05-OBJ.obj");
    Model bunnyModel((char *)"assets/bunny.ply");
    Model sphereModel((char *)"assets/sphere.ply");

    // cityShader.use();
    // unsigned int idx = 0;
    // float w = 200.0;
    // for (float x=-15; x<15; x+=1) {
    //     for (float z=-15; z<15; z+=1) {
    //         std::stringstream ss;
    //         string index;
    //         ss << ++idx;
    //         index = ss.str();
    //         string label = ("offsets[" + index + "]").c_str();
    //         cityShader.setVec2(label, x*w, z*w);
    //     }
    // }

    glm::vec3 LookTarget;

    bool keyDown = false;

    while(!glfwWindowShouldClose(window))
    {
        scenePredraw();

        while (!metronomeQueue->empty()) {
            MetronomeEvent event = metronomeQueue->back();
            metronomeQueue->pop_back();
            pulseHeight = 1.0f;
        }

        while (!midiNoteQueue->empty()) {
            MidiNoteEvent event = midiNoteQueue->back();
            midiNoteQueue->pop_back();
            std::cout << "note: " << event.note << " velocity: " << event.velocity << std::endl;
            if (event.note == 36 && event.velocity > 0) {
                pulseHeight = (float)event.velocity / 127.0f;
            }
            cityShader.setInt("selected", event.note);
        }

        while (!envelopeQueue->empty()) {
            EnvelopeEvent event = envelopeQueue->back();
            envelopeQueue->pop_back();
            pulseHeight = event.value;
        }

        pulseHeight *= 0.95f;//60.0f * deltaTime;

        float rotationAngle = 10.0f * t;

        glm::mat4 model;
        glm::mat4 view;
        model = glm::translate(model, glm::vec3(0.0, -10000.0, 0.0));
        model = glm::scale(model, glm::vec3(100000.0));

        // handle additional keys with some janky debouncing
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && ! keyDown) {
            keyDown = true;
            camera.FreeLook = ! camera.FreeLook;
            LookTarget = glm::vec3(10.0f, 0.0f, 0.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
            keyDown = false;
        }

        if (!camera.FreeLook) {
            //LookTarget != NULL && LookTarget)
            // glm::quat quatStart (glm::lookAt (eye_start, center_start, up_start));
            // glm::quat quatStart = view;

            // glm::vec3 upEnd(0.0f, 1.0f, 0.0f);
            // glm::vec3 centerEnd = camera.Position;
            // glm::vec3 eyeEnd = glm::normalize(LookTarget - camera.Position);
            // glm::quat quatEnd (glm::lookAt(eyeEnd, centerEnd, upEnd));

            float w = 1.0f/10;
            glm::vec3 Position(1000 * glm::cos(w*t), 1000.0f, 1000 * glm::sin(w*t));
            glm::vec3 LookTarget(0.0, 0.0, 0.0);
            glm::vec3 UpVector(0.0, 1.0, 0.0);

            view = glm::lookAt(
                Position,
                LookTarget,
                UpVector
            );

            glm::quat rot = glm::angleAxis(deltaTime * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            camera.Orientation = rot * camera.Orientation;
            view = glm::mat4();
            view = glm::translate(view, glm::vec3(0.0f, -100 + -t*10, 0.0f)) * glm::toMat4(camera.Orientation);

            // view = glm::toMat4(glm::mix(glm::toQuat(camera.view), toQuat(view), 0.5));
            // view = SlerpRot(view, quatEnd, camera.Position, camera.Position, 0.5f);
            // SlerpRot(quatStart, quatEnd, 0.5f);
            /*
            glm::mat4 SlerpRot(glm::quat quatStart, glm::quat quatEnd, glm::vec3 eyeStart, glm::vec3 eyeEnd, float amount) {
            // hat tip: https://stackoverflow.com/questions/27185055/how-to-implement-a-smooth-transition-between-two-different-camera-view-in-opengl

            // First interpolate the rotation
            glm::quat quatInterp = glm::slerp (quatStart, quatEnd, amount);

            // Then interpolate the translation
            // glm::vec3 posInterp  = glm::mix   (eyeStart,  eyeEnd,  amount);
            // glm::mat4 view_matrix = glm::mat4_cast (quatInterp); // Setup rotation
            // view_matrix[3]        = glm::vec4 (posInterp, 1.0);  // Introduce translation
            // return view_matrix;
            return glm::mat4_cast(quatInterp);
        }
            */
        } else {
            view = camera.GetViewMatrix();
        }

        geometryShader.use();

        geometryShader.setFloat("t", t);
        geometryShader.setFloat("f", 10.0f);
        geometryShader.setFloat("pulseHeight", pulseHeight);

        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);
        geometryShader.setMat4("model", model);

        // plane.draw();
        // plane.drawLines();
        // plane.drawPoints();

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0,0.0, 0.0));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0, 10.0, 10.0));
        lineShader.use();
        lineShader.setFloat("t", t);
        lineShader.setFloat("pulseHeight", pulseHeight);
        // lineShader.setMat4("MVP", projection * view * model);
        lineShader.setMat4("projection", projection);
        lineShader.setMat4("view", view);
        lineShader.setMat4("model", model);

        lineShader.use();
        // bunnyModel.Draw(lineShader);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0,0.0, 0.0));
        // model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(10.0, 10.0, 10.0));

        // // bunny
        Mesh bunnyMesh = bunnyModel.meshes[0];
        glBindVertexArray(bunnyMesh.VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyMesh.EBO);

        // "normal" mode bunny

        bunnyShader.use();
        bunnyShader.setFloat("t", t);
        bunnyShader.setMat4("projection", projection);
        bunnyShader.setMat4("view", view);
        bunnyShader.setMat4("model", model);
        bunnyModel.Draw(bunnyShader);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, bunnyMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0,0.0, 0.0));
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f));
        bunnyShader.setMat4("model", model);
        Mesh sphereMesh = sphereModel.meshes[0];
        // glBindVertexArray(sphereMesh.VAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereMesh.EBO);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // glDrawElements(GL_TRIANGLES, sphereMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

        // // lines only
        // bunnyLineShader.use();
        // bunnyLineShader.setMat4("projection", projection);
        // bunnyLineShader.setMat4("view", view);
        // bunnyLineShader.setMat4("model", model);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

        // draw city w/ instanced shader
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));

        cityShader.use();
        cityShader.setFloat("t", t);
        cityShader.setFloat("cameraX", camera.Position.x);
        cityShader.setFloat("cameraZ", camera.Position.z);
        cityShader.setFloat("pulseHeight", pulseHeight);

        // cityShader.setMat4("projection", projection);
        // cityShader.setMat4("view", view);
        // cityShader.setMat4("model", model);
        cityShader.setMat4("MVP", projection * view * model);

        Mesh cityMesh = cityModel.meshes[0];
        glBindVertexArray(cityMesh.VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cityMesh.EBO);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // glDrawElements(GL_TRIANGLES, cityMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
        glDrawElementsInstanced(GL_TRIANGLES, cityMesh.indices.size(), GL_UNSIGNED_INT, (void*)0, 900);

        scenePostdraw();
    }

    glfwTerminate();
    return 0;
}
