#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Target;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float FOV;
    float AspectRatio;
    float NearPlane;
    float FarPlane;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    void UpdateAspectRatio(int width, int height);

private:
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;

    void updateCameraVectors();
};

#endif