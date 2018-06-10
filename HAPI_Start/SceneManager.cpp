#include "SceneManager.hpp"
#include "GraphicsSystem.hpp"
#include "MainMenu.hpp"
#include "Simulation.hpp"

SceneManager::SceneManager(SIMULATION::Simulation& sim) : m_sim(&sim)
{
	this->m_currentScene = new MainMenu();
	this->m_currentScene->loadScene(*this);
}


SceneManager::~SceneManager()
{
	//Delete current pointer if valid
	if (this->m_currentScene != nullptr)
	{
		this->m_currentScene->unloadSceneData();
		delete this->m_currentScene;
	}

	//Delete all stored pointers
	for (auto it = this->m_StoredScenes.begin(); it != this->m_StoredScenes.end();)
	{
		if (it->second != nullptr)
		{
			it->second->unloadSceneData();
			delete it->second;
			it = this->m_StoredScenes.erase(it);
		} else 
			it++;
	}
}

void SceneManager::updateCScene()
{
	this->m_currentScene->updateScene();
}

void SceneManager::drawCScene(float interV)
{
	this->m_currentScene->drawScene(interV);
}

BaseScene* SceneManager::changeScene(BaseScene& scene, bool unloadOld)
{
	BaseScene* temp = this->m_currentScene;
	scene.loadScene(*this);
	this->m_currentScene = &scene;
	if(unloadOld)
		temp->unloadSceneData();
	return temp;
}

bool SceneManager::storeScene(std::string name, BaseScene& sceneToStore)
{
	if (this->m_StoredScenes.find(name) == this->m_StoredScenes.end())
	{
		this->m_StoredScenes[name] = &sceneToStore;
		return true;
	}
	return false;
}

BaseScene* SceneManager::retriveStoredScene(std::string name)
{
	if (this->m_StoredScenes.find(name) != this->m_StoredScenes.end())
	{
		BaseScene* scene = this->m_StoredScenes.at(name);
		this->m_StoredScenes.erase(name);
		return scene;
	}
	return nullptr;
}

bool SceneManager::deleteStoreScene(std::string name)
{
	if (this->m_StoredScenes.find(name) != this->m_StoredScenes.end())
	{
		this->m_StoredScenes.erase(name);
		return true;
	}
	return false;
}

SIMULATION::Simulation& SceneManager::getSimulation() const
{
	return (*this->m_sim);
}
