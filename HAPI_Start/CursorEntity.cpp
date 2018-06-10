#include "CursorEntity.h"
#include "GraphicsSystem.hpp"
#include "UIElement.hpp"
#include "Enemy.hpp"
#include "EnemyInfoPanel.h"
#include "SoundManager.h"

CursorEntity::CursorEntity(EnemyInfoPanel* eip,std::string identifier, Rectangle collisionVolume, int frameNum) : Entity(identifier, collisionVolume, frameNum),
																							  m_mouseData(HAPI.GetMouseData()),
																							  m_selectedEnemy(nullptr),
																							  m_infoPanel(eip),
																							  m_mouseDownLastUpdate(false),
																							  m_clicked(false)
{
	this->m_side = eSide::Cursor;
	if(this->m_infoPanel != nullptr)
		this->m_infoPanel->setActive(false);
}


CursorEntity::~CursorEntity()
{
}

void CursorEntity::collision(Entity & other)
{
	if (other.getSide() == eSide::Enemy)
	{
		//Handle enemy selection 
		Enemy& enemy = (Enemy&)other;
		//Show enemy information if clicked
		if (this->m_clicked)
		{
			if (this->m_selectedEnemy != nullptr)
			{
				this->m_selectedEnemy->setSelected(false);
			}
			this->m_selectedEnemy = &enemy;
			this->m_clicked = false;
			this->m_selectedEnemy->setSelected(true);
			if (this->m_infoPanel != nullptr)
			{
				this->m_infoPanel->setActive(true);
			}
		}
 	}
	else if (other.getSide() == eSide::UI)
	{
		//Cursor is over UI element
		UIElement& ui = (UIElement&)other;
		//Set entered as we are inside
		ui.setEntered(true);
		ui.setPressed(this->m_mouseData.leftButtonDown);
	}
}

void CursorEntity::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;
	//Update the mouse position
	this->setPosition(this->m_mouseData.x + cameraRect.getLeft(), m_mouseData.y + cameraRect.getTop(), false);

	this->m_clicked = false;

	//Check if clicked
	if (this->m_mouseDownLastUpdate && !this->m_mouseData.leftButtonDown)
	{
		this->m_clicked = true;
	}
	this->m_mouseDownLastUpdate = this->m_mouseData.leftButtonDown;
	
	//Update the panel info. (We will check if the info panel is nulllpointer as the cursor entity also silves in mainMenu where no info panel should exists)
	if (this->m_infoPanel != nullptr)
	{
		//Check if we got a selecte enemy to uptade, if none is selecte then just disable the info panel
		if (this->m_selectedEnemy == nullptr)
		{
			this->m_infoPanel->setActive(false);
		}
		else
		{
			//Update the panel
			//Check if the enemy is alive or not
			if (this->m_selectedEnemy->isActive())
			{
				this->m_infoPanel->setHp(this->m_selectedEnemy->getHP(), this->m_selectedEnemy->getMaxHP());
				this->m_infoPanel->setMP(this->m_selectedEnemy->getMP(), this->m_selectedEnemy->getMaxMP());
				this->m_infoPanel->setCurrentLevel(this->m_selectedEnemy->getLevel());
			}
			else
			{
				//Enemy is dead disable panel
				this->m_infoPanel->setActive(false);
				this->m_selectedEnemy = nullptr;
			}
		}
	}
}

const Entity * const CursorEntity::getSelectedEnemy()
{
	return this->m_selectedEnemy;
}
