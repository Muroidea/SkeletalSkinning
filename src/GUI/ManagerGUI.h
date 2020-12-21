#ifndef MANAGERGUI_H
#define MANAGERGUI_H

#include "Managers/ModelManager.h"
#include "Managers/AnimationManager.h"

class ManagerGUI
{
public:
	static void DrawModels(ModelManager *manager);
	static void DrawAnimations(AnimationManager *manager);
};

#endif // !MANAGERGUI_H