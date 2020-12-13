#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "Manager.h"
#include "Animation/Animation.h"

class AnimationManager : public Manager<Animation>
{
public:
	AnimationManager(std::string loadPath);
	~AnimationManager();

	virtual bool Load(std::string filename) override;
};

#endif // !ANIMATIONMANAGER_H
