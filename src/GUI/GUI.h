//
// Created by Rafał on 28.11.2020.
//

#ifndef GUI_H
#define GUI_H

#include "Window.h"
#include "GameObject.h"
#include "Managers/ModelManager.h"
#include "Managers/AnimationManager.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class GUI
{
private:
    unsigned int m_Width;
    unsigned int m_Height;

    bool m_Enabled = true;

	GameObject *m_SelectedNode = nullptr;

public:
    GUI();
    ~GUI();

    bool Init(const Window* window);
    void Update(float deltaTime);
    void Render(GameObject *rootNode, ModelManager *modelManager, AnimationManager *animationManager);
    void Switch();

    void Resize(unsigned int width, unsigned int height);

	static bool DrawVec3(const std::string& labelID, glm::vec3& vector, float moveStep);
};


#endif // !GUI_H
