#ifndef MANAGER_H
#define MANAGER_H

#include <map>
#include <string>

template <typename T>
class Manager
{
protected:
	std::map<std::string, T*> m_Dictionary;
	const std::string m_LoadPath;

	Manager(std::string loadPath);
	~Manager();

public:
	T* Get(std::string key);

	bool IsLoaded(std::string);
	virtual bool Load(std::string filename) = 0;

	std::size_t Size();

	friend class ManagerGUI;
};

template <typename T>
Manager<T>::Manager(std::string loadPath)
	: m_LoadPath(loadPath)
{
	this->m_Dictionary = std::map<std::string, T*>();
}

template<typename T>
Manager<T>::~Manager()
{
	for (auto i : m_Dictionary)
		delete i.second;
}

template<typename T>
T* Manager<T>::Get(std::string key)
{
	if (IsLoaded(key))
		return m_Dictionary[key];
	else
		return nullptr;
}

template <typename T>
bool Manager<T>::IsLoaded(std::string key)
{
	return m_Dictionary.find(key) != m_Dictionary.end();
}

template<typename T>
std::size_t Manager<T>::Size()
{
	return m_Dictionary.size();
}

#endif // !MANAGER_H
