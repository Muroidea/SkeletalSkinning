//
// Created by Rafał on 28.11.2020.
//

#include "GUI.h"

#include "Application.h"

GUI::GUI()
{
}

GUI::~GUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool GUI::Init(const Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    const char* glsl_version = "#version 430";

    m_Width = window->GetWidth();
    m_Height = window->GetHeight();

    bool res = ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
    if (!res)
    {
        printf("Failed to initialize ImGui for Glfw\n");
        return false;
    }

    res = ImGui_ImplOpenGL3_Init(glsl_version);
    if (!res)
    {
        printf("Failed to initialize ImGui for OpenGL\n");
        return false;
    }

    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().ChildRounding = 0.0f;
    ImGui::GetStyle().FrameRounding = 0.0f;
    ImGui::GetStyle().GrabRounding = 0.0f;
    ImGui::GetStyle().PopupRounding = 0.0f;
    ImGui::GetStyle().ScrollbarRounding = 0.0f;

    return true;
}

void GUI::Update(float deltaTime)
{

}

void GUI::Render(GameObject* rootNode, ModelManager* modelManager, AnimationManager* animationManager)
{
    if (!m_Enabled) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Skinning Menu", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f), true);
    ImGui::SetWindowSize(ImVec2((m_Width * 0.3f) > 400.0f ? 400.0f : (m_Width * 0.3f), m_Height));

	ImGui::BeginTabBar("Menu", ImGuiTabBarFlags_::ImGuiTabBarFlags_None);

	if (ImGui::BeginTabItem("Scene"))
	{
        ImGui::LabelText("##comboScenarioLabel", "Scenarios:");

        const char* items[] = { AppState::s_ScenarioName[0].c_str(), AppState::s_ScenarioName[1].c_str(), AppState::s_ScenarioName[2].c_str() };
        static int item_current = AppState::s_ChoosenScenario;
        if (ImGui::Combo("##comboScenario", &item_current, items, IM_ARRAYSIZE(items)))
        {
            AppState::s_ChoosenScenario = item_current;
            std::invoke(AppState::s_Scenarios[item_current], *Application::Get());
            m_SelectedNode = nullptr;
        }

        ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

		GameObjectGUI::DrawTree(rootNode, m_SelectedNode);
            
        if (m_SelectedNode)
        {
            ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

			GameObjectGUI::DrawNode(m_SelectedNode);
        }
            
        ImGui::EndTabItem();
    }

    if (!AppState::s_SequenceMode)
        if (ImGui::BeginTabItem("Models"))
        {
		    ManagerGUI::DrawModels(modelManager);

		    if (m_SelectedNode)
		    {
			    ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

			    GameObjectGUI::DrawModel(m_SelectedNode);
		    }

            ImGui::EndTabItem();
        }

    if (ImGui::BeginTabItem("Animations"))
    {
        if (!AppState::s_SequenceMode)
        {
            ManagerGUI::DrawAnimations(animationManager);

            if (m_SelectedNode)
            {
                ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();

                GameObjectGUI::DrawAnimationState(m_SelectedNode);
            }

            ImGui::NewLine(); ImGui::Separator(); ImGui::NewLine();
        }

        GameObjectGUI::DrawActionsForAll(rootNode);

        ImGui::EndTabItem();
    }

	ImGui::EndTabBar();

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

	//bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::Switch()
{
    m_Enabled = !m_Enabled;
}

void GUI::Resize(unsigned int width, unsigned int height)
{
    m_Width = width;
    m_Height = height;
}