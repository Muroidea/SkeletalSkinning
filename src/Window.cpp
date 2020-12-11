//
// Created by Rafał on 28.11.2020.
//

#include "Window.h"
#include "Application.h"

Window::Window(const std::string & title, unsigned int width, unsigned int height)
    : m_Title(title), m_Width(width), m_Height(height)
{
    
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::Init()
{
    int success = glfwInit();
    if (!success)
    {
        printf("Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_Window = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);

    // Set callbacks
    glfwSetWindowSizeCallback(m_Window, Application::OnWindowResize);
    glfwSetCursorPosCallback(m_Window, Input::MousePosCallback);
    glfwSetKeyCallback(m_Window, Input::KeyCallback);

    // Set mouse mode
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Load OpenGL functions
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!status)
    {
        printf("Failed to initialize GLAD\n");
        return false;
    }

    return true;
}

void Window::Resize(unsigned int width, unsigned int height)
{
    m_Width = width;
    m_Height = height;
}

void Window::SwitchCursorVisibility()
{
    if (m_CursorVisibility)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_CursorVisibility = false;
    }
    else
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_CursorVisibility = true;
    }
}

void Window::ProcessInput()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}