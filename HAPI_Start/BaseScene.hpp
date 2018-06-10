#pragma once
#include <vector>

class Entity;
namespace GRAPHICS
{
	class GraphicsSystem;
};
namespace SOUND
{
	class SoundManager;
};
class SceneManager;
class BaseScene
{
public:
	BaseScene();
	virtual ~BaseScene();

	virtual void updateScene() = 0;
	virtual void drawScene(float interV) = 0;
	virtual void loadScene(SceneManager& sm) = 0;
	virtual void unloadSceneData() = 0;

protected:
	SceneManager* m_sceneM;
	std::vector<Entity*> m_entities;
	bool m_isLoaded;
};

