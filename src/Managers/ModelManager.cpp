#include "ModelManager.h"

#include <imgui.h>

ModelManager::ModelManager(std::string loadPath)
	: Manager(loadPath)
{
}

ModelManager::~ModelManager()
{
}

bool ModelManager::Load(std::string filename)
{
    std::string name = filename.substr(0, filename.find_last_of("."));

    if (IsLoaded(name))
        return false;

    Model *newModel = new Model(name);

    newModel->LoadModel(m_LoadPath + filename);
    m_Dictionary[name] = newModel;

    return true;
}

void ModelManager::RenderGUI()
{
	float width = ImGui::GetWindowWidth();
	for (auto model : m_Dictionary)
	{
		ImGui::Button(model.first.c_str(), ImVec2(width, 20.0f));

		long long address = reinterpret_cast<long long>(model.second);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("Model", &address, sizeof(long long));
			ImGui::Text("%s", model.second->GetName().c_str());
			ImGui::EndDragDropSource();
		}
	}
}