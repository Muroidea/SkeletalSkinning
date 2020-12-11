#include <iostream>

#include "Application.h"

int main()
{
    Application *app = Application::Get("SkeletalSkinning");
    app->Run();

    return 0;
}