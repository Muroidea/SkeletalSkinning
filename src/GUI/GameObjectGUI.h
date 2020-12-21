
#ifndef GAMEOBJECTGUI_H
#define GAMEOBJECTGUI_H

#include <glm/glm.hpp>

#include "GameObject.h"
#include "Model.h"
#include "Animation/AnimationState.h"

class GameObjectGUI
{
public:
	static void DrawTree(GameObject *root, GameObject *&selectedNode);
	static void DrawNode(GameObject *node);

	static void DrawModel(GameObject *node);
	static void DrawAnimationState(GameObject *node);
	static void DrawActionsForAll(GameObject* node);

private:
	static void DrawTreeRecursive(GameObject *node, GameObject *&selectedNode);
	static bool DrawVec3(const std::string& labelID, glm::vec3& vector, float moveStep);
};

#endif // !GAMEOBJECTGUI_H