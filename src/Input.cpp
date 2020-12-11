
#include "Input.h"

Input* Input::m_Instance = nullptr;

Input::Input()
{

}

Input::~Input()
{

}

void Input::PreUpdate()
{
	for (auto key : m_ReleasedKeys)
		m_CurrentKeys.erase(key);
}

void Input::PostUpdate()
{
	m_PressedKeys.clear();
	m_ReleasedKeys.clear();

	m_LastMousePosX = m_CurrentMousePosX;
	m_LastMousePosY = m_CurrentMousePosY;
}

Input* Input::Get()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new Input();
	}

	return m_Instance;
}

bool Input::IsKeyActive(int keyCode)
{
	return Input::Get()->m_CurrentKeys.find(keyCode) != Input::Get()->m_CurrentKeys.end();
}

bool Input::IsKeyPressed(int keyCode)
{
	return Input::Get()->m_PressedKeys.find(keyCode) != Input::Get()->m_PressedKeys.end();
}

bool Input::IsKeyReleased(int keyCode)
{
	return Input::Get()->m_ReleasedKeys.find(keyCode) != Input::Get()->m_ReleasedKeys.end();
}

glm::vec2 Input::GetMousePos()
{
	return glm::vec2(Input::Get()->m_CurrentMousePosX, Input::Get()->m_CurrentMousePosY);
}

float Input::GetMouseDeltaPosX()
{
	return Input::Get()->m_CurrentMousePosX - Input::Get()->m_LastMousePosX;
}

float Input::GetMouseDeltaPosY()
{
	return Input::Get()->m_CurrentMousePosY - Input::Get()->m_LastMousePosY;
}

glm::vec2 Input::GetMouseDeltaPos()
{
	return glm::vec2(GetMouseDeltaPosX(), GetMouseDeltaPosY());
}

void Input::MousePosCallback(GLFWwindow* window, double axisX, double axisY)
{
	Input::Get()->m_CurrentMousePosX = axisX;
	Input::Get()->m_CurrentMousePosY = axisY;

	if (Input::Get()->m_FirstMove)
	{
		Input::Get()->m_FirstMove = false;
		Input::Get()->m_LastMousePosX = axisX;
		Input::Get()->m_LastMousePosY = axisY;
	}
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
		Input::Get()->m_CurrentKeys.insert(key);
		Input::Get()->m_PressedKeys.insert(key);
		break;
	case GLFW_RELEASE:
		Input::Get()->m_ReleasedKeys.insert(key);
		break;
	case GLFW_REPEAT:
		break;
	}
}
