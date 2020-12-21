#include "ManagerGUI.h"

#include <imgui.h>

void ManagerGUI::DrawModels(ModelManager * manager)
{
	ImGui::NewLine();

	ImGui::BeginChild("SceneModels", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight() * 0.2f), false, ImGuiWindowFlags_HorizontalScrollbar);

	for (auto record : manager->m_Dictionary)
	{
		ImGui::Button(record.first.c_str(), ImVec2(ImGui::GetWindowWidth(), 20.0f));

		long long address = reinterpret_cast<long long>(record.second);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("Model", &address, sizeof(long long));
			ImGui::Text("%s", record.second->GetName().c_str());
			ImGui::EndDragDropSource();
		}
	}

	ImGui::EndChild();
}

void ManagerGUI::DrawAnimations(AnimationManager * manager)
{
	ImGui::NewLine();

	ImGui::BeginChild("SceneAnimations", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight() * 0.2f), false, ImGuiWindowFlags_HorizontalScrollbar);

	for (auto record : manager->m_Dictionary)
	{
		ImGui::Button(record.first.c_str(), ImVec2(ImGui::GetWindowWidth(), 20.0f));

		long long address = reinterpret_cast<long long>(record.second);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("Animation", &address, sizeof(long long));
			ImGui::Text("%s", record.second->GetName().c_str());
			ImGui::EndDragDropSource();
		}
	}

	ImGui::EndChild();
}
