
#ifndef INPUT_H
#define INPUT_H

#include <unordered_set>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Input
{
private:
	static Input* m_Instance;

	bool m_FirstMove = true;

	float m_CurrentMousePosX = 0.0f;
	float m_CurrentMousePosY = 0.0f;

	float m_LastMousePosX = 0.0f;
	float m_LastMousePosY = 0.0f;

	std::unordered_set<int> m_CurrentKeys;
	std::unordered_set<int> m_PressedKeys;
	std::unordered_set<int> m_ReleasedKeys;

private:
	Input();
	~Input();

public:
	void PreUpdate();
	void PostUpdate();

public:
	static Input* Get();

	static bool IsKeyActive(int keyCode);
	static bool IsKeyPressed(int keyCode);
	static bool IsKeyReleased(int keyCode);

	static glm::vec2 GetMousePos();

	static float GetMouseDeltaPosX();
	static float GetMouseDeltaPosY();
	static glm::vec2 GetMouseDeltaPos();

	static void MousePosCallback(GLFWwindow *window, double axisX, double axisY);
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
};

#endif // !INPUT_H
