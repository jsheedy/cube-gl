#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum CameraActions {
    CENTER_ROT,
    CENTER_HOVER,
    HOVER_BUNNY,
    FREELOOK,
};

// Default camera values
const float YAW        = -90.0f;
const float PITCH      =  0.0f;
const float SPEED      =  2.5f;
const float SENSITIVTY =  0.005f;
const float ZOOM       =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 Front;
    glm::vec3 WorldUp;

    glm::mat4 view;
    glm::quat Orientation;

    CameraActions Action;

    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = glm::vec3(0.0, 1.0, 0.0);
        Up = glm::vec3(0.0, 1.0, 0.0);
        Front = glm::vec3(0.0, 0.0, 1.0);
        Right = glm::vec3(1.0, 0.0, 0.0);
        Yaw = yaw;
        Pitch = pitch;
        Orientation = glm::quat();
        Action = FREELOOK;
        // updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        Orientation = glm::quat();
        Action = FREELOOK;
        // updateCameraVectors();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += (glm::inverse(Orientation) * Front) * velocity;
            // Position += glm::vec3(glm::mat4_cast(Orientation) * glm::vec4((Front * velocity), 1.0f));
        if (direction == BACKWARD)
            Position -= (glm::inverse(Orientation) * Front) * velocity;
        if (direction == LEFT)
            Position -= (glm::inverse(Orientation) * Right) * velocity;
        if (direction == RIGHT)
            Position += (glm::inverse(Orientation) * Right) * velocity;
        if (direction == UP)
            Position += (glm::inverse(Orientation) * Up) * velocity;
        if (direction == DOWN)
            Position -= (glm::inverse(Orientation) * Up) * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        glm::quat YawRot = glm::angleAxis(xoffset, Up);
        glm::quat PitchRot = glm::angleAxis(yoffset, Right);

        Orientation =  PitchRot * Orientation * YawRot;
    }

    void LookAt(glm::vec3 target) {
        glm::vec3 forward = target - Position;
        Front = glm::normalize(forward);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));

        // h/t https://stackoverflow.com/questions/23337151/trouble-converting-matrix-to-quaternion-and-back
        // glm::vec3 lookAtPt = direction;
        glm::mat4 rotMatrix = glm::lookAt(glm::vec3(0), Front, WorldUp);
        Orientation = glm::quat_cast(rotMatrix);
    }

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif