#pragma once
#include "Entity.hpp"

class Enemy;
class EnemyInfoPanel;

class CursorEntity :
	public Entity
{
public:
	CursorEntity(EnemyInfoPanel* eip, std::string identifier, Rectangle collisionVolume, int frameNum = 0);
	virtual ~CursorEntity();

	virtual void collision(Entity& other) override final;
	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override final;

	const Entity * const getSelectedEnemy();
private:
	const HAPI_TMouseData& m_mouseData;
	Enemy* m_selectedEnemy;
	EnemyInfoPanel* m_infoPanel;
	bool m_mouseDownLastUpdate;
	bool m_clicked;
};

