//
// Created by Rafik on 30.11.2020.
//


#include "Camera.h"
#include "Input.h"

Camera::Camera(glm::vec3 position, float movementSpeed, float mouseSensitivity)
        : m_Freezed(false), m_Position(position), m_Forward(1.0f, 0.0f, 0.0f), m_Up(0.0f, 1.0f, 0.0f), m_Right(), m_WorldUp(m_Up),
          m_MovementSpeed(movementSpeed), m_MouseSensitivity(mouseSensitivity)
{
    UpdateCameraVectors();
}

Camera::~Camera()
{

}

void Camera::SetMovementSpeed(float movementSpeed)
{
    m_MovementSpeed = movementSpeed;
}

void Camera::SetMouseSensitivity(float mouseSensitivity)
{
    m_MouseSensitivity = mouseSensitivity;
}

void Camera::SetFreezed(bool freezed)
{
    m_Freezed = freezed;
}

void Camera::ProcessMouseInput(float deltaTime)
{
    if (m_Freezed) return;

    float axisX = m_MouseSensitivity * Input::GetMouseDeltaPosX();
    float axisY = m_MouseSensitivity * Input::GetMouseDeltaPosY();

    m_Yaw += axisX;
    m_Pitch -= axisY;

    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;

    UpdateCameraVectors();
}

void Camera::ProcessKeyboardInput(float deltaTime)
{
    if (m_Freezed) return;

    glm::vec3 direction(0.0f);
    if (Input::IsKeyActive(GLFW_KEY_W))
        direction += glm::vec3(1.0f, 0.0f, 0.0f);
    if (Input::IsKeyActive(GLFW_KEY_S))
        direction += glm::vec3(-1.0f, 0.0f, 0.0f);
    if (Input::IsKeyActive(GLFW_KEY_A))
        direction += glm::vec3(0.0f, 0.0f, 1.0f);
    if (Input::IsKeyActive(GLFW_KEY_D))
        direction += glm::vec3(0.0f, 0.0f, -1.0f);

    float velocity = m_MovementSpeed * deltaTime;

    if (direction.x == 1.0f)
        m_Position += m_Forward * velocity;
    if (direction.x == -1.0f)
        m_Position -= m_Forward * velocity;
    if (direction.z == 1.0f)
        m_Position -= m_Right * velocity;
    if (direction.z == -1.0f)
        m_Position += m_Right * velocity;
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 forward;
    forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    forward.y = sin(glm::radians(m_Pitch));
    forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Forward = glm::normalize(forward);

    m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
}

