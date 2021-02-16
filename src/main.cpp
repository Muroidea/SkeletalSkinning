#include <iostream>

#include "Application.h"

#include "DataStructures/KDTree.h"

#include "glm/gtx/string_cast.hpp"

int main()
{
    /*
    std::vector<glm::vec3> data{
        { 1.0f,  0.0f,  2.0f },
        { 2.0f,  1.0f,  1.0f },
        {-1.0f,  2.0f,  6.0f },
        { 7.0f, -5.0f, -3.0f }
    };

    KDTree<3, glm::vec3> kdTree(data);
    kdTree.Insert({ 0.0f, 3.0f, -1.0f });
    kdTree.Insert({ 2.0f,  1.0f,  0.0f });
    kdTree[{ 8.0f, 4.0f, 7.0f }];

    kdTree.Empty();
    if (kdTree.Contains({ 2.0f,  1.0f,  1.0f })) printf("y \n");
    if (!kdTree.Contains({ 2.0f,  1.0f,  0.0f })) printf("n \n");

    printf("%s \n", glm::to_string(kdTree.kNNValue({ 2.0f,  1.0f,  1.0f }, 2)[1]).c_str());
    */

    Application *app = Application::Get("SkeletalSkinning");
    app->Run();

    return 0;
}