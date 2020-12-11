#include "AnimationManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <imgui.h>

AnimationManager::AnimationManager(std::string loadPath)
	: Manager(loadPath)
{
}

AnimationManager::~AnimationManager()
{
}

bool AnimationManager::Load(std::string filename)
{
    std::string name = filename.substr(0, filename.find_last_of("."));

    if (IsLoaded(name))
        return false;

    Assimp::Importer importer;
    const aiScene* scene;

    scene = importer.ReadFile(m_LoadPath + filename,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices);

    for (int i = 0; i < scene->mNumAnimations; i++)
    {
        Animation* newAnimation = new Animation(name);
        newAnimation->LoadAnimation(scene->mAnimations[i]);
        m_Dictionary[name] = newAnimation;
    }

	return true;
}

void AnimationManager::RenderGUI()
{
    float width = ImGui::GetWindowWidth();
    for (auto anim : m_Dictionary)
    {
        ImGui::Button(anim.first.c_str(), ImVec2(width, 20.0f));

        long long address = reinterpret_cast<long long>(anim.second);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("Animation", &address, sizeof(long long));
            ImGui::Text("%s", anim.second->GetName().c_str());
            ImGui::EndDragDropSource();
        }
    }
}
