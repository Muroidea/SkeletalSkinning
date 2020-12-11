
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>

#include "Model.h"
#include "Animation/AnimationState.h"

class GameObject
{
private:
	static unsigned int s_Counter;

	unsigned int m_ID;

	glm::mat4 m_Local = glm::mat4(1.0f);
	glm::mat4 m_Global = glm::mat4(1.0f);

	bool m_DirtyFlag = true;
	bool m_Enabled = true;

	GameObject* m_Parent;
	std::vector<GameObject*> m_Children;

public:
	Model *m_Model;
	AnimationState *m_AnimationState;

public:
	GameObject(GameObject* parent, Model* model, AnimationState* animationState);
	~GameObject();

	void Update(bool dirtyFlag = false);
	void Update(bool dirtyFlag, const glm::mat4& parent);
	void AddChild(GameObject* child);
	void SetAnimationState(AnimationState* animationState);

	int GetNumChildren();
	const std::vector<GameObject*>& GetChildren();

	glm::mat4 GetLocalMatrix();
	glm::mat4 GetGlobalMatrix();

	void SetLocalTransform(glm::mat4 transform);

	glm::vec3 GetLocalPosition();
	void SetLocalPosition(glm::vec3 position);

	glm::quat GetLocalRotation();
	void SetLocalRotation(glm::quat rotation);

	glm::vec3 GetLocalScale();
	void SetLocalScale(glm::vec3 localScale);

	bool GetEnabled() const;
	void SetEnabled(bool enabled);

	void DrawTreeGUIRoot(GameObject *&selectedNode);
	void DrawTreeGUIRecursive(GameObject *&selectedNode);
	void DrawNodeGUI();

	bool operator==(const GameObject& other);
};

#endif // !GAMEOBJECT_H