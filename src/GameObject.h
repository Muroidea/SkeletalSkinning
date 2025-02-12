
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>

#include "Animation/AnimationSequence.h"
#include "Animation/AnimationState.h"
#include "Model.h"

class GameObject
{
private:
	static unsigned int s_Counter;
	unsigned int m_ID;

	std::string m_Name;

	glm::vec3 m_LocalPosition = glm::vec3(0.0f);
	glm::vec3 m_LocalRotation = glm::vec3(0.0f);
	glm::vec3 m_LocalScale = glm::vec3(1.0f);

	glm::mat4 m_Global = glm::mat4(1.0f);

	bool m_DirtyFlag = true;
	bool m_Enabled = true;

	GameObject* m_Parent = nullptr;
	GameObject* m_ChildToRemove = nullptr;
	std::vector<GameObject*> m_Children;

public:
	Model *m_Model;
	bool m_RenderRed;

	AnimationState *m_AnimationState;
	AnimationSequence *m_AnimationSequence;

public:
	GameObject(std::string name = "GameObject", Model * model = nullptr, AnimationState* animationState = nullptr);
	~GameObject();

	void Update(bool dirtyFlag = false, const glm::mat4& parent = glm::mat4(1.0f));

	void AddChild(GameObject* child);
	void SetToRemove();

	void SetAnimationState(AnimationState* animationState);

	int GetNumChildren();
	const std::vector<GameObject*>& GetChildren();

	glm::mat4 GetLocalMatrix();
	glm::mat4 GetGlobalMatrix();

	std::string GetName() const { return m_Name; }

	glm::vec3 GetLocalPosition();
	void SetLocalPosition(glm::vec3 position);

	glm::vec3 GetLocalRotation();
	void SetLocalRotation(glm::vec3 rotation);

	glm::vec3 GetLocalScale();
	void SetLocalScale(glm::vec3 scale);

	bool GetEnabled() const;
	void SetEnabled(bool enabled);

	template <typename Functor>
	void DoForAll(Functor& functor);

	bool operator==(const GameObject& other);

private:
	void RemoveChild();

	friend class GameObjectGUI;
};

template <typename Functor>
void GameObject::DoForAll(Functor& functor)
{
	if (!m_Enabled) return;

	functor(*this);

	for (int i = 0; i < m_Children.size(); i++)
		m_Children[i]->DoForAll(functor);
}

#endif // !GAMEOBJECT_H