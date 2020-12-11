//
// Created by Rafał on 28.11.2020.
//

#include "GUI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

    bool show_demo_window = true;
    bool show_another_window = false;

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

    bool show_demo_window = true;
    bool show_another_window = true;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Skinning Menu", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f), true);
    ImGui::SetWindowSize(ImVec2((m_Width * 0.3f) > 400.0f ? 400.0f : (m_Width * 0.3f), m_Height));

    if (ImGui::BeginTabBar("Menu", ImGuiTabBarFlags_::ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Scene"))
        {
            ImGui::NewLine();

            ImGui::BeginChild("SceneTreeChild", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.8f, m_Height * 0.2f), false, ImGuiWindowFlags_HorizontalScrollbar);

            rootNode->DrawTreeGUIRoot(m_SelectedNode);

            ImGui::EndChild();
            
            if (m_SelectedNode)
            {
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::NewLine();
                m_SelectedNode->DrawNodeGUI();
            }
            
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Models"))
        {
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Animations"))
        {
            ImGui::NewLine();
            ImGui::BeginChild("SceneAnimations", ImVec2(ImGui::GetWindowContentRegionWidth(), m_Height * 0.2f), false, ImGuiWindowFlags_HorizontalScrollbar);

            animationManager->RenderGUI();

            ImGui::EndChild();

            if (m_SelectedNode)
            {
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::NewLine();

                m_SelectedNode->m_AnimationState->RenderGUI();
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
    /*
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }*/

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
