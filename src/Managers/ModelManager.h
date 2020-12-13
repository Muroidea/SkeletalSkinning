#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "Manager.h"
#include "Model.h"

class ModelManager : public Manager<Model>
{
public:
	ModelManager(std::string loadPath);
	~ModelManager();

	virtual bool Load(std::string filename) override;
};

#endif // !MODELMANAGER_H