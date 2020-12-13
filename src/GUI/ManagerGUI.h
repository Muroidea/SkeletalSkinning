#ifndef MANAGERGUI_H
#define MANAGERGUI_H

#include "Managers/ModelManager.h"
#include "Managers/AnimationManager.h"

class ManagerGUI
{
public:
	static void Draw(ModelManager *manager);
	static void Draw(AnimationManager *manager);
};

#endif // !MANAGERGUI_H