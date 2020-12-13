
#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

Application* Application::s_Instance = nullptr;

Application* Application::Get(std::string title, unsigned int width, unsigned int height)
{
	if (s_Instance == nullptr)
	{
		s_Instance = new Application(title, width, height);
	}	

	return s_Instance;
}

Application::Application(std::string title, unsigned int width, unsigned int height)
	: m_Running(true)
{
	m_Window = new Window(title, width, height);
	m_Renderer = new Renderer();
	m_GUI = new GUI();
	m_Input = Input::Get();
	m_Camera = new Camera(glm::vec3(0.0f, 200.0f, 300.0f), 150.0f);

	m_ModelManager = new ModelManager("../resources/models/");
	m_AnimationManager = new AnimationManager("../resources/animations/");

	m_RootNode = new GameObject("root");
}

Application::~Application()
{
}

void Application::InitScene()
{
	//Load models
	m_ModelManager->Load("Akai.fbx");
	m_ModelManager->Load("Warrok.fbx");

	//Warrok animation
	m_AnimationManager->Load("Warrok_OrcIdle.fbx");
	m_AnimationManager->Load("Warrok_WarriorIdle.fbx");
	m_AnimationManager->Load("Warrok_StandingRunForward.fbx");

	//Akai animation
	m_AnimationManager->Load("Akai_StandingRunForward.fbx");
	
	m_RootNode->AddChild(new GameObject("Akai 1", m_ModelManager->Get("Akai")));
	m_RootNode->AddChild(new GameObject("Warrok 1", m_ModelManager->Get("Warrok")));
}

void Application::Run()
{
	float deltaTime = 0.0f;
	float lastFrame = glfwGetTime();
	float currentFrame;

	OnInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	while (m_Running)
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		m_Window->ProcessInput();
		OnWindowShouldClose();

		m_Input->PreUpdate();

		OnUpdate(deltaTime);
		OnRender(deltaTime);

		m_Input->PostUpdate();

		m_Window->SwapBuffers();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	OnShutdown();
}

void Application::OnInit()
{
	if (!m_Window->Init())
	{
		printf("Window initialization failed\n");
		m_Running = false;
	}
	
	if (!m_Renderer->Init())
	{
		printf("Renderer initialization failed\n");
		m_Running = false;
	}

	if (!m_GUI->Init(m_Window))
	{
		printf("GUI initialization failed\n");
		m_Running = false;
	}

	InitScene();
	m_Renderer->Resize(m_Window->GetWidth(), m_Window->GetHeight());
}

void Application::OnShutdown()
{
}

void Application::OnUpdate(float dt)
{
	if (Input::IsKeyPressed(GLFW_KEY_F1))
	{
		m_Camera->SetFreezed(!m_Camera->Freezed());

		if (m_Camera->Freezed())
			glfwSetInputMode(m_Window->GetWindow() , GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (Input::IsKeyPressed(GLFW_KEY_F2))
		m_GUI->Switch();

	m_Camera->ProcessKeyboardInput(dt);
	m_Camera->ProcessMouseInput(dt);

	m_Renderer->UpdateCamera(m_Camera);
	m_RootNode->Update();
}

void Application::OnRender(float dt)
{
	m_Renderer->Render(m_RootNode, dt);
	m_GUI->Render(m_RootNode, m_ModelManager, m_AnimationManager);
}

void Application::OnWindowShouldClose()
{
	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE) ||
		glfwWindowShouldClose(m_Window->GetWindow()))
		m_Running = false;
}

void Application::OnWindowResize(GLFWwindow *window, int width, int height)
{
	Application* app = Application::Get();
	app->m_Window->Resize(width, height);
	app->m_Renderer->Resize(width, height);
	app->m_GUI->Resize(width, height);

	glViewport(0, 0, app->m_Window->GetWidth(), app->m_Window->GetHeight());
}
