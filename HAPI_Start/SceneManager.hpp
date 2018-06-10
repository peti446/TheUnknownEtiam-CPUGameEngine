#pragma once
#include <unordered_map>
#include <string>
#include "BaseScene.hpp"

namespace GRAPHICS
{
	class GraphicsSystem;
};
namespace SIMULATION
{
	class Simulation;
};

class SceneManager
{
public:
	SceneManager(SIMULATION::Simulation& sim);
	~SceneManager();

	void updateCScene();
	void drawCScene(float interV);
	
	BaseScene* changeScene(BaseScene& scene, bool unloadOld);
	bool storeScene(std::string name, BaseScene& sceneToStore);

	BaseScene* retriveStoredScene(std::string name);
	bool deleteStoreScene(std::string name);

	//
	SIMULATION::Simulation& getSimulation() const;
private:
	BaseScene* m_currentScene;
	SIMULATION::Simulation* m_sim;
	std::unordered_map<std::string, BaseScene*> m_StoredScenes;
};

