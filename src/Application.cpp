
#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <fstream>

bool AppState::s_SequenceMode = false;
bool AppState::s_Subdivide = false;
int AppState::s_ChoosenScenario = 1;
void (Application::* AppState::s_Scenarios[3])() = { &Application::Scenario1, &Application::Scenario3, &Application::Scenario2 };
std::string AppState::s_ScenarioName[3] = { "Akai arm test", "Warrok arm test", "Sign language synthesis 1" };

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
	std::ifstream file("../resources/config.txt", std::ifstream::in);
	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			if (line.find("subdivide") != std::string::npos)
			{
				auto it = line.find("=");
				std::string res = line.substr(it + 2);
				if (res == "true") AppState::s_Subdivide = true;
			}
		}
	}

	//Load models
	m_ModelManager->Load("Akai.fbx");
	m_ModelManager->Load("Warrok.fbx");
	m_ModelManager->Load("Willie.fbx");

	//Willie animation
	m_AnimationManager->Load("Willie_WillieTest.fbx");

	//Warrok animation
	m_AnimationManager->Load("Warrok_OrcIdle.fbx");
	m_AnimationManager->Load("Warrok_WarriorIdle.fbx");
	m_AnimationManager->Load("Warrok_StandingRunForward.fbx");
	m_AnimationManager->Load("Warrok_ArmTest.fbx");

	//Akai animation
	m_AnimationManager->Load("Akai_Instytut.fbx");
	m_AnimationManager->Load("Akai_Informatyki.fbx");
	m_AnimationManager->Load("Akai_Jest.fbx");
	m_AnimationManager->Load("Akai_Jednostka.fbx");
	m_AnimationManager->Load("Akai_Naukowo.fbx"); 
	m_AnimationManager->Load("Akai_Badawcza.fbx");
	m_AnimationManager->Load("Akai_Politechniki.fbx");
	m_AnimationManager->Load("Akai_Lodzkiej.fbx");
	m_AnimationManager->Load("Akai_ArmTest.fbx");
	m_AnimationManager->Load("Akai_StandingRunForward.fbx");

	std::invoke(AppState::s_Scenarios[AppState::s_ChoosenScenario], Application::Get());
}

void Application::ClearScene()
{
	delete m_RootNode;
	m_RootNode = new GameObject("root");
}


void Application::Scenario1()
{
	ClearScene();
	AppState::s_SequenceMode = false;

	m_Renderer->Reset();

	// Prepare scene elements
	auto model = m_ModelManager->Get("Akai");
	auto animation = m_AnimationManager->Get("Akai_ArmTest");


	AnimationState* animState = new AnimationState(animation);
	animState->SetSkinningType(SkinningType::LINEAR_BLEND_SKINNING);
	GameObject* go = new GameObject("object 1", model, animState);

	go->SetLocalPosition(glm::vec3(-150.0f, 0.0f, 0.0f));

	m_RootNode->AddChild(go);


	animState = new AnimationState(animation);
	animState->SetSkinningType(SkinningType::DUAL_QUATERNION_SKINNING);

	go = new GameObject("object 2", model, animState);

	go->SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	m_RootNode->AddChild(go);


	animState = new AnimationState(animation);
	animState->SetSkinningType(SkinningType::CENTERS_OF_ROTATION_SKINNING);

	go = new GameObject("object 3", model, animState);
	
	go->SetLocalPosition(glm::vec3(150.0f, 0.0f, 0.0f));

	m_RootNode->AddChild(go);
}

void Application::Scenario2()
{
	ClearScene();
	AppState::s_SequenceMode = true;

	m_Renderer->Reset();

	// Prepare scene elements
	auto model = m_ModelManager->Get("Akai");

	GameObject* go = new GameObject("object 1", model);

	go->m_AnimationSequence->SetSkinningType(SkinningType::LINEAR_BLEND_SKINNING);
	go->m_AnimationSequence->SetLoop(true);

	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Instytut")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Informatyki")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Politechniki")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Lodzkiej")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Jest")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Jednostka")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Naukowo")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Badawcza")));
	go->m_AnimationSequence->SetEnabled(true, true);
	go->m_AnimationSequence->SetEnabled(false);

	go->SetLocalPosition(glm::vec3(-110.0f, 0.0f, 0.0f));
	//go->SetLocalRotation(glm::vec3(180.0f, 0.0f, 0.0f));
	//go->SetLocalScale(glm::vec3(50.0f, 50.0f, 50.0f));

	m_RootNode->AddChild(go);


	go = new GameObject("object 2", model);

	go->m_AnimationSequence->SetSkinningType(SkinningType::DUAL_QUATERNION_SKINNING);
	go->m_AnimationSequence->SetLoop(true);

	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Instytut")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Informatyki")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Politechniki")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Lodzkiej")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Jest")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Jednostka")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Naukowo")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Badawcza")));
	go->m_AnimationSequence->SetEnabled(true, true);
	go->m_AnimationSequence->SetEnabled(false);

	go->SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//go->SetLocalRotation(glm::vec3(180.0f, 0.0f, 0.0f));
	//go->SetLocalScale(glm::vec3(50.0f, 50.0f, 50.0f));

	m_RootNode->AddChild(go);


	go = new GameObject("object 3", model);

	go->m_AnimationSequence->SetSkinningType(SkinningType::CENTERS_OF_ROTATION_SKINNING);
	go->m_AnimationSequence->SetLoop(true);

	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Instytut")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Informatyki")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Politechniki")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Lodzkiej")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Jest")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Jednostka")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Naukowo")));
	go->m_AnimationSequence->AddAnimation(new AnimationState(m_AnimationManager->Get("Akai_Badawcza")));
	go->m_AnimationSequence->SetEnabled(true, true);
	go->m_AnimationSequence->SetEnabled(false);

	go->SetLocalPosition(glm::vec3(110.0f, 0.0f, 0.0f));
	//go->SetLocalRotation(glm::vec3(180.0f, 0.0f, 0.0f));
	//go->SetLocalScale(glm::vec3(50.0f, 50.0f, 50.0f));

	m_RootNode->AddChild(go);
}

void Application::Scenario3()
{
	ClearScene();
	AppState::s_SequenceMode = false;

	m_Renderer->Reset();

	// Prepare scene elements
	auto model = m_ModelManager->Get("Warrok");
	auto animation = m_AnimationManager->Get("Warrok_ArmTest");


	AnimationState* animState = new AnimationState(animation);
	animState->SetSkinningType(SkinningType::LINEAR_BLEND_SKINNING);
	GameObject* go = new GameObject("object 1", model, animState);

	go->SetLocalPosition(glm::vec3(-200.0f, 0.0f, 0.0f));

	m_RootNode->AddChild(go);


	animState = new AnimationState(animation);
	animState->SetSkinningType(SkinningType::DUAL_QUATERNION_SKINNING);

	go = new GameObject("object 2", model, animState);

	go->SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	m_RootNode->AddChild(go);


	animState = new AnimationState(animation);
	animState->SetSkinningType(SkinningType::CENTERS_OF_ROTATION_SKINNING);

	go = new GameObject("object 3", model, animState);

	go->SetLocalPosition(glm::vec3(200.0f, 0.0f, 0.0f));

	m_RootNode->AddChild(go);
}

void Application::Run()
{
	float deltaTime = 0.0f;
	float lastFrame = glfwGetTime();
	float currentFrame;

	OnInit();

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
