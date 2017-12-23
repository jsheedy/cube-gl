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

float ROT_SLERP_MIX = 0.1f;

glm::vec3 UpVector(0.0f, 1.0f, 0.0f);


float angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 origin) {
    glm::vec3 da=glm::normalize(a-origin);
    glm::vec3 db=glm::normalize(b-origin);
    return glm::acos(glm::dot(da, db));
}

glm::quat FromToRotation(glm::vec3 start, glm::vec3 dest){
    /* thx
    http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/#how-do-i-find-the-rotation-between-2-vectors-
    */
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f){
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
        // FIXME:
		// if (glm::gtx::norm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
		// 	rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = glm::cross(start, dest);

	float s = glm::sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);

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

    glm::vec3 LookTarget;

    bool keyDown = false;

    while(!glfwWindowShouldClose(window))
    {
        scenePredraw(camera);

        while (!metronomeQueue->empty()) {
            MetronomeEvent event = metronomeQueue->back();
            metronomeQueue->pop_back();
            pulseHeight = 1.0f;
        }

        while (!midiNoteQueue->empty()) {
            MidiNoteEvent event = midiNoteQueue->back();
            midiNoteQueue->pop_back();
            if (event.note == 36 && event.velocity > 0) {
                pulseHeight = (float)event.velocity / 127.0f;
            }
            if (event.velocity == 0) {
                cityShader.setInt("selected", 0);
            } else {
                cityShader.setInt("selected", event.note);
            }
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
            camera.Action = CENTER_ROT;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
            keyDown = false;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && ! keyDown) {
            keyDown = true;
            camera.Action = CENTER_HOVER;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
            keyDown = false;
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && ! keyDown) {
            keyDown = true;
            camera.Action = HOVER_BUNNY;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
            keyDown = false;
        }

        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && ! keyDown) {
            keyDown = true;
            camera.Action = FREELOOK;
        }
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
            keyDown = false;
        }

        if (camera.Action == CENTER_ROT) {

            glm::vec3 TargetPosition(0.0f, 1000.0f, 0.0f);
            glm::vec3 RightVector(1.0f, 0.0f, 0.0f);

            glm::quat rotAround = glm::angleAxis(glm::radians(20.0f * t), UpVector);
            glm::quat rotDown = glm::angleAxis(glm::radians(45.0f), RightVector);
            glm::quat rot = rotDown * rotAround * glm::quat();
            camera.Position = glm::mix(camera.Position, TargetPosition, ROT_SLERP_MIX);
            camera.Orientation = glm::mix(camera.Orientation, rot, ROT_SLERP_MIX);
        }
        else if (camera.Action == CENTER_HOVER) {
            glm::vec3 TargetPosition(0.0f, 2000.0f, 0.0f);
            glm::quat rot = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            camera.Position = glm::mix(camera.Position, TargetPosition, ROT_SLERP_MIX);
            camera.Orientation = glm::mix(camera.Orientation, rot, ROT_SLERP_MIX);
        }
        else if (camera.Action == HOVER_BUNNY) {
            glm::vec3 TargetPosition(10.0f * sin(t), 10.0f, 10.0f * cos(t));
            glm::quat rot = glm::angleAxis(0.0f, UpVector);
            camera.Position = glm::mix(camera.Position, TargetPosition, ROT_SLERP_MIX);
            camera.Orientation = glm::mix(camera.Orientation, rot, ROT_SLERP_MIX);
        }
        else if (camera.Action == FREELOOK) {
            //  ¯\_(ツ)_/¯
            // slerp rotation to up so we're not rotated
            // glm::vec3 up = glm::inverse(camera.Orientation) * glm::vec3(0.0, 1.0, 0.0);
            // glm::quat q = FromToRotation(up, glm::vec3(0.0, 1.0, 0.0));
            // glm::quat rot = glm::mix(camera.Orientation, q, ROT_SLERP_MIX);
            // camera.Orientation = rot * camera.Orientation;
        }

        view = glm::toMat4(camera.Orientation);
        view = glm::translate(view, -camera.Position);

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
        lineShader.setMat4("projection", projection);
        lineShader.setMat4("view", view);
        lineShader.setMat4("model", model);

        lineShader.use();
        bunnyModel.Draw(lineShader);

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
        bunnyLineShader.use();
        bunnyLineShader.setMat4("projection", projection);
        bunnyLineShader.setMat4("view", view);
        bunnyLineShader.setMat4("model", model);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, bunnyMesh.indices.size(), GL_UNSIGNED_INT, (void*)0);

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
