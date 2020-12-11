//
// Created by Rafik on 30.11.2020.
//


#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    bool m_Freezed;

    glm::vec3 m_Position;
    glm::vec3 m_Forward;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;

    float m_MovementSpeed;
    float m_MouseSensitivity;

public:
    Camera(glm::vec3 position, float movementSpeed = 20.0f, float mouseSensitivity = 0.1f);
    ~Camera();

    void SetMovementSpeed(float movementSpeed);
    void SetMouseSensitivity(float mouseSensitivity);
    void SetFreezed(bool freezed);

    void ProcessMouseInput(float deltaTime);
    void ProcessKeyboardInput(float deltaTime);

    inline glm::mat4 GetViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Forward, m_Up); }
    inline glm::vec3 Position() const { return m_Position; }
    inline glm::vec3 Forward() const { return m_Forward; }
    inline glm::vec3 Up() const { return m_Up; }
    inline bool Freezed() const { return m_Freezed; }

private:
    void UpdateCameraVectors();
};

#endif // !CAMERA_H
