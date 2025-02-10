#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
    : Position(position), Target(target), WorldUp(worldUp),
    Yaw(-90.0f), Pitch(0.0f), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(45.0f),
    FOV(45.0f), AspectRatio(800.0f / 600.0f), NearPlane(0.1f), FarPlane(100.0f)
{
    updateCameraVectors();
}

// ---------- Obtention de la matrice de vue ----------
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Target, WorldUp);
}

// ---------- Obtention de la matrice de projection ----------
glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(FOV), AspectRatio, NearPlane, FarPlane);
}

// ---------- Mise à jour du ratio d'aspect ----------
void Camera::UpdateAspectRatio(int width, int height)
{
    AspectRatio = (float)width / (float)height;
}

// ---------- Mise à jour des vecteurs de la caméra ----------
void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
