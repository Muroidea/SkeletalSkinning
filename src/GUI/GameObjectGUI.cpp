#include "GameObjectGUI.h"

#include <imgui.h>
#include <imgui/imgui_internal.h>

void GameObjectGUI::DrawTree(GameObject *root, GameObject *&selectedNode)
{
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (ImGui::Button("Add", ImVec2(80.0f, 20.0f)))
		root->AddChild(new GameObject());

	ImGui::SameLine();

	if (!selectedNode)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

		ImGui::Button("Remove", ImVec2(80.0f, 20.0f));

		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	else if (ImGui::Button("Remove", ImVec2(80.0f, 20.0f)))
	{
		selectedNode->SetToRemove();
		selectedNode = nullptr;
	}

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	ImGui::BeginChild("SceneTreeChild", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.8f, ImGui::GetWindowHeight() * 0.2f), false, ImGuiWindowFlags_HorizontalScrollbar);

	for (int i = 0; i < root->m_Children.size(); i++)
	{
		DrawTreeRecursive(root->m_Children[i], selectedNode);
	}

	ImGui::EndChild();
}

void GameObjectGUI::DrawTreeRecursive(GameObject *node, GameObject *&selectedNode)
{
	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	ImGuiTreeNodeFlags nodeFlags = baseFlags;
	bool isSelected = false;

	if (selectedNode && node) isSelected = *selectedNode == *node;
	if (isSelected) nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (node->GetNumChildren() > 0)
	{
		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)node, nodeFlags, "%s", node->m_Name.c_str());

		if (ImGui::IsItemClicked())
			selectedNode = node;

		if (nodeOpen)
		{
			for (int i = 0; i < node->m_Children.size(); i++)
			{
				DrawTreeRecursive(node->m_Children[i], selectedNode);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)node, nodeFlags, "%s", node->m_Name.c_str());

		if (ImGui::IsItemClicked())
			selectedNode = node;
	}
}

void GameObjectGUI::DrawNode(GameObject *node)
{
	glm::vec3 position = node->GetLocalPosition();
	glm::vec3 rotation = node->GetLocalRotation();
	glm::vec3 scale = node->GetLocalScale();

	float float_low = -1000000.0f, float_high = 1000000.0f;

	char buffer[25];
	strcpy(buffer, node->m_Name.c_str());
	ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.0f);
	if (ImGui::InputText("##Name", buffer, (int)(sizeof(buffer) / sizeof(*buffer))))
		node->m_Name = buffer;
	ImGui::PopItemWidth();

	ImGui::SameLine();

	if (ImGui::Button("Add child", ImVec2(80.0f, 20.0f)))
		node->AddChild(new GameObject());

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	ImGui::Checkbox(" Enable/Disable", &node->m_Enabled);

	ImGui::NewLine();
	ImGui::Text("Local transform: "); ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 1.0f));
	if (ImGui::Button("Reset", ImVec2(80.0f, 15.0f)))
	{
		node->SetLocalPosition(glm::vec3(0.0f));
		node->SetLocalRotation(glm::vec3(0.0f));
		node->SetLocalScale(glm::vec3(1.0f));
	}
	ImGui::PopStyleVar(1);

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (DrawVec3("Position", position, 1.0f))
		node->SetLocalPosition(position);

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (DrawVec3("Rotation", rotation, 0.2f))
		node->SetLocalRotation(rotation);

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (DrawVec3("Scale", scale, 0.1f))
		node->SetLocalScale(scale);
}

void GameObjectGUI::DrawModel(GameObject* node)
{
	bool hasModel = (node->m_Model) ? true : false;

	ImGui::Text("Model: ");

	{ // Model slot
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.1f, 0 });

		std::string buttonLabel = (hasModel) ? node->m_Model->GetName() : "Model (empty)";
		ImGui::Button(buttonLabel.c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f, 20.0f));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Model"))
			{
				IM_ASSERT(payload->DataSize == sizeof(long long));
				long long address = *(const long long*)payload->Data;
				node->m_Model = reinterpret_cast<Model*>(address);
			}
			ImGui::EndDragDropTarget();
		}
	}
	
	ImGui::SameLine();

	{ // Clear button
		if (!hasModel)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}

		if (ImGui::Button("Clear", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 20.0f)))
			node->m_Model = nullptr;

		if (!hasModel)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar(1);
		}
	}

	ImGui::PopStyleVar(1);
}

void GameObjectGUI::DrawAnimationState(GameObject * node)
{
	AnimationState *animState = node->m_AnimationState;
	Animation *animation = animState->GetAnimation();
	bool hasAnimation = (animation) ? true : false;

	ImGui::Text("Animation state: "); ImGui::SameLine();

	ImGui::NewLine();

	{ // Animation slot
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.1f, 0 });

		std::string buttonLabel = (hasAnimation) ? animation->GetName() : "Animation (empty)";
		ImGui::Button(buttonLabel.c_str(), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.6f, 20.0f));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Animation"))
			{
				IM_ASSERT(payload->DataSize == sizeof(long long));
				long long address = *(const long long*)payload->Data;
				animState->SetAnimation(reinterpret_cast<Animation*>(address));
			}
			ImGui::EndDragDropTarget();
		}
	}

	ImGui::SameLine();

	{ // Clear button
		if (!hasAnimation)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}

		if (ImGui::Button("Clear", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 20.0f)))
			animState->SetAnimation(nullptr);

		if (!hasAnimation)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar(1);
		}
	}
	
	ImGui::PopStyleVar(1);

	if (hasAnimation)
	{
		ImGui::NewLine();

		ImGui::Checkbox("Enable/Disable", &animState->m_Enabled); ImGui::SameLine();
		ImGui::Checkbox("Play in loop", &animState->m_Loop);

		ImGui::NewLine();

		ImGui::Text("Duration in ticks: %f", animation->GetDuration());
		ImGui::Text("Duration in seconds: %.2f", animation->GetDuration() / animation->GetTicksPerSecond());
		ImGui::Text("Ticks per second: %f", animation->GetTicksPerSecond());

		ImGui::NewLine();

		ImGui::DragFloat("Time scale", &animState->m_TimeScale, 0.1f, 0.1f, 20.0f, "%.1f", ImGuiSliderFlags_None);
		ImGui::SliderFloat("Time in ticks", &animState->m_LocalTime, 0.0f, animation->GetDuration(), "%.1f");
	}
}

bool GameObjectGUI::DrawVec3(const std::string & labelID, glm::vec3 & vector, float moveStep)
{
	bool changed = false;
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { 20.0f, lineHeight };
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	ImGui::PushID(labelID.c_str());

	ImGui::Text(labelID.c_str());
	ImGui::Dummy(ImVec2(0.0f, 4.0f));

	ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
	ImGui::Button("X", buttonSize);
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##X", &vector.x, moveStep, 0.0f, 0.0f, "%.2f"))
		changed = true;

	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f });
	ImGui::Button("Y", buttonSize);
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##Y", &vector.y, moveStep, 0.0f, 0.0f, "%.2f"))
		changed = true;
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.0f, 1.0f, 1.0f });
	ImGui::Button("Z", buttonSize);
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##Z", &vector.z, moveStep, 0.0f, 0.0f, "%.2f"))
		changed = true;
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::PopID();

	return changed;
}
