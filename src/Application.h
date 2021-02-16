//
// Created by Rafał on 28.11.2020.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer.h"
#include "Camera.h"
#include "Window.h"
#include "Input.h"
#include "GUI/GUI.h"

#include "Managers/ModelManager.h"
#include "Managers/AnimationManager.h"

class Application;

class AppState
{
public:
    static bool s_SequenceMode;
    static bool s_Subdivide;
    static int s_ChoosenScenario;
    static void (Application::*s_Scenarios[3])();
    static std::string s_ScenarioName[3];
};

class Application
{
private:
    static Application* s_Instance;

    Window *m_Window;
    Renderer *m_Renderer;
    GUI *m_GUI;
    Input *m_Input;

    Camera *m_Camera;
    ModelManager *m_ModelManager;
    AnimationManager *m_AnimationManager;

    GameObject* m_RootNode;

    bool m_Running;

public:
    static Application* Get(std::string title = "Skinning", unsigned int width = 1280, unsigned int height = 720);

    void Run();

    void Scenario1();
    void Scenario2();
    void Scenario3();

private:
    Application(std::string title, unsigned int width, unsigned int height);
    ~Application();

    void InitScene();
    void ClearScene();

    void OnInit();
    void OnShutdown();
    void OnUpdate(float dt);
    void OnRender(float dt);
    void OnWindowShouldClose();

public:
    static void OnWindowResize(GLFWwindow *window, int width, int height);
};


#endif // !APPLICATION_H
