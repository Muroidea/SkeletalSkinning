//
// Created by Rafał on 28.11.2020.
//

#ifndef WINDOW_H
#define WINDOW_H

#include "Camera.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

#include <glm/glm.hpp>

class Window
{
private:
    GLFWwindow *m_Window;

    std::string m_Title;
    unsigned int m_Width, m_Height;
    bool m_CursorVisibility;

    bool m_FirstMouse = true;
    glm::vec2 m_LastPos;

public:
    Window(const std::string& title, unsigned int width, unsigned int height);
    ~Window();

    bool Init();
    void Resize(unsigned int width, unsigned int height);
    void SwitchCursorVisibility();

    void ProcessInput();
    void SwapBuffers();

    inline unsigned int GetWidth() const { return m_Width; }
    inline unsigned int GetHeight() const { return m_Height; }
    inline GLFWwindow* GetWindow() const { return m_Window; }
};


#endif // !WINDOW_H
