#pragma once
#include <vector>
#include <future>
#include <queue>
#include "BaseScene.hpp"
#include "Vector2.h"
#include "SpellEntity.hpp"

class Entity;
class TMap;

class GameScene : public BaseScene
{
public:
	GameScene();
	virtual ~GameScene();

	virtual void updateScene() override final;
	virtual void drawScene(float interV) override final;
	virtual void loadScene(SceneManager& sm) override final;
	virtual void unloadSceneData() override final;

	void spawnSpell(const float damage, const Entity* const owner, const Entity* const target, const SpellEntity::eSpellType type, const Vector2 dir, const std::string identifier, const DWORD frameTimeMS = 1000, const int frameNumStart = 0, const int frameNumEnd = 0);
	void spawnRandomEnemy(const int level);
	const DWORD getTickInterval() const;

	void playerDied();

private:
	TMap* m_currentMap;
	int m_rowsToDraw;
	int m_columnsToDraw;
	Entity* m_cursorEntity;
	Entity const* m_PlayerEntity;
	std::vector<Entity*> m_deadEnemies;
	std::vector<Entity*> m_spells;
	std::queue<std::pair<int, Entity*>> m_entitiesToMove;
	DWORD m_lastTimeSpawned;
};

