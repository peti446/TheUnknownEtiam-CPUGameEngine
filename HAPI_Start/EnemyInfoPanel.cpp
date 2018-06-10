#include "EnemyInfoPanel.h"
#include "GraphicsSystem.hpp"
#include "ProgresBar.hpp"
#include "SoundManager.h"


EnemyInfoPanel::EnemyInfoPanel(GRAPHICS::GraphicsSystem & gs, const Vector2& pos) : UIElement("eipBG", Rectangle(0,230,0,95))
{
	//Set up background
	gs.LoadTexture("eipBG", "Data/UI/panel_boltsBlue.png");
	gs.resizeTexture("eipBG", 230, 95);

	//Name
	this->addText("Enenmy", HAPI_TColour::BLACK, 20, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Top, 20, 15);

	//Set up bars
	//HP
	gs.LoadTexture("hpBarLeft", "Data/UI/barRed_horizontalLeft.png");
	gs.LoadTexture("hpBarMid", "Data/UI/barRed_horizontalMid.png");
	gs.LoadTexture("hpBarRight", "Data/UI/barRed_horizontalRight.png");
	this->m_hpBar = new ProgresBar("hpBarLeft", "hpBarMid", "hpBarRight", 130, gs.getRect("hpBarLeft").getHeight(), gs);
	this->addText("Hp:", HAPI_TColour::BLACK, 18, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Top, 25, 40);
	this->m_hpBar->setPosition(70, 35, false);

	//Mana
	gs.LoadTexture("mpBarLeft", "Data/UI/barBlue_horizontalLeft.png");
	gs.LoadTexture("mpBarMid", "Data/UI/barBlue_horizontalMid.png");
	gs.LoadTexture("mpBarRight", "Data/UI/barBlue_horizontalRight.png");
	this->m_manaBar = new ProgresBar("mpBarLeft", "mpBarMid", "mpBarRight", 130, gs.getRect("mpBarLeft").getHeight(), gs);
	this->addText("Mp:", HAPI_TColour::BLACK, 18, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Top, 25, 61);
	this->m_manaBar->setPosition(70, 56, false);

	//Level
	this->addText("Level: 1", HAPI_TColour::BLACK, 18, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Right, Text::eUITextAlignmentVert::Top, -20, 17);
	
	//Set position
	Vector2 p = pos;
	p.x += 5;
	this->setPosition(pos, false);
}

EnemyInfoPanel::~EnemyInfoPanel()
{
	delete this->m_hpBar;
	delete this->m_manaBar;
}

void EnemyInfoPanel::setHp(const int cv, const int maxValue)
{
	this->m_hpBar->setValue(cv, maxValue);
}

void EnemyInfoPanel::setMP(const int cv, const int maxValue)
{
	this->m_manaBar->setValue(cv, maxValue);
}

void EnemyInfoPanel::setCurrentLevel(int cl)
{
	this->m_texts.back().setText("Level: " + std::to_string(cl));
}

void EnemyInfoPanel::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;
	this->m_hpBar->update(cameraRect, sm);
	this->m_manaBar->update(cameraRect, sm);
	UIElement::update(cameraRect, sm);
}

void EnemyInfoPanel::draw(const GRAPHICS::GraphicsSystem & gs, float interV)
{
	if (!this->m_active)
		return;
	//Draw the background and the text
	UIElement::draw(gs, interV);
	//Draw the progres bars
	this->m_hpBar->draw(gs, interV);
	this->m_manaBar->draw(gs, interV);
}

void EnemyInfoPanel::setActive(bool active)
{
	Entity::setActive(active);
	this->m_hpBar->setActive(active);
	this->m_manaBar->setActive(active);
}

void EnemyInfoPanel::setPosition(const Vector2 & pos, const bool inter)
{
	Vector2 hpPos = this->m_hpBar->getPosition() - this->m_hpBar->getPreviousPosition();
	Vector2 panelTransfer = pos - this->m_position;
	if (hpPos != panelTransfer)
		this->m_hpBar->setPosition(pos + (this->m_hpBar->getPosition() - this->m_position), inter);
	Vector2 mpPosTransfer = this->m_manaBar->getPosition() - this->m_manaBar->getPreviousPosition();
	if (mpPosTransfer != panelTransfer)
		this->m_manaBar->setPosition(pos + (this->m_manaBar->getPosition() - this->m_position), inter);
	Entity::setPosition(pos, inter);
	Entity::setPosition(pos, inter);
}

