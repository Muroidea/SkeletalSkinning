#include "ModelManager.h"

ModelManager::ModelManager(std::string loadPath)
	: Manager(loadPath)
{
}

ModelManager::~ModelManager()
{
}

bool ModelManager::Load(std::string filename)
{
    std::string name = filename.substr(0, filename.find_last_of("."));

    if (IsLoaded(name))
        return false;

    Model *newModel = new Model(name);

    newModel->LoadModel(m_LoadPath + filename);
    m_Dictionary[name] = newModel;

    return true;
}