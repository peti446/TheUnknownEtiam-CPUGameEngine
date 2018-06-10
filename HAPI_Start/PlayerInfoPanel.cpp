#include "PlayerInfoPanel.hpp"
#include "GraphicsSystem.hpp"
#include "SoundManager.h"


PlayerInfoPanel::PlayerInfoPanel(GRAPHICS::GraphicsSystem& gs) : UIElement("pipBG", Rectangle(0, 230,0,115))
{
	//Set up background
	gs.LoadTexture("pipBG", "Data/UI/panel_boltsBlue.png");
	gs.resizeTexture("pipBG", 230, 115);

	//Name
	this->addText("Ethiam", HAPI_TColour::BLACK, 20, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Top, 20, 15);

	//Set up bars
	//HP
	gs.LoadTexture("hpBarLeft", "Data/UI/barRed_horizontalLeft.png");
	gs.LoadTexture("hpBarMid", "Data/UI/barRed_horizontalMid.png");
	gs.LoadTexture("hpBarRight", "Data/UI/barRed_horizontalRight.png");
	this->m_hpBar = new ProgresBar("hpBarLeft", "hpBarMid", "hpBarRight", 130, gs.getRect("hpBarLeft").getHeight(), gs);
	this->addText("Hp:", HAPI_TColour::BLACK, 18, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Top, 25, 40);
	this->m_hpBar->setPosition(70, 39,false);

	//Mana
	gs.LoadTexture("mpBarLeft", "Data/UI/barBlue_horizontalLeft.png");
	gs.LoadTexture("mpBarMid", "Data/UI/barBlue_horizontalMid.png");
	gs.LoadTexture("mpBarRight", "Data/UI/barBlue_horizontalRight.png");
	this->m_manaBar = new ProgresBar("mpBarLeft", "mpBarMid", "mpBarRight", 130, gs.getRect("mpBarLeft").getHeight(), gs);
	this->addText("Mp:", HAPI_TColour::BLACK, 18, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Top, 25, 61);
	this->m_manaBar->setPosition(70, 60, false);

	//XP
	gs.LoadTexture("xpBarLeft", "Data/UI/barPurple_horizontalLeft.png");
	gs.LoadTexture("xpBarMid", "Data/UI/barPurple_horizontalMid.png");
	gs.LoadTexture("xpBarRight", "Data/UI/barPurple_horizontalRight.png");
	this->m_xpBar = new ProgresBar("xpBarLeft", "xpBarMid", "xpBarRight", 130, gs.getRect("xpBarLeft").getHeight(), gs);
	this->addText("XP:", HAPI_TColour::BLACK, 17, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Top, 25, 82);
	this->m_xpBar->setPosition(70, 79, false);

	//Level
	this->addText("Level: 1", HAPI_TColour::BLACK, 18, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Right, Text::eUITextAlignmentVert::Top, -20, 17);

	//Set position of the info panel
	Entity::setPosition(5, 5, false);
}


PlayerInfoPanel::~PlayerInfoPanel()
{
	delete this->m_xpBar;
	delete this->m_manaBar;
	delete this->m_hpBar;
}

void PlayerInfoPanel::setHp(const int cv, const int maxValue)
{
	this->m_hpBar->setValue(cv, maxValue);
}

void PlayerInfoPanel::setMP(const int cv, const int maxValue)
{
	this->m_manaBar->setValue(cv, maxValue);
}

void PlayerInfoPanel::setXP(const int cv, const int maxValue)
{
	this->m_xpBar->setValue(cv, maxValue);
}

void PlayerInfoPanel::setCurrentLevel(int cl)
{
	this->m_texts.back().setText("Level: " + std::to_string(cl));
}

void PlayerInfoPanel::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;
	this->m_hpBar->update(cameraRect, sm);
	this->m_manaBar->update(cameraRect, sm);
	this->m_xpBar->update(cameraRect, sm);
	UIElement::update(cameraRect, sm);
}

void PlayerInfoPanel::draw(const GRAPHICS::GraphicsSystem & gs, float interV)
{
	if (!this->m_active)
		return;
	//Draw the background and the text
	UIElement::draw(gs, interV);

	//Draw the progres bars
	this->m_hpBar->draw(gs, interV);
	this->m_manaBar->draw(gs, interV);
	this->m_xpBar->draw(gs, interV);
}

void PlayerInfoPanel::setActive(bool active)
{
	Entity::setActive(active);
	this->m_hpBar->setActive(active);
	this->m_manaBar->setActive(active);
	this->m_xpBar->setActive(active);
}

void PlayerInfoPanel::setPosition(const Vector2 & pos, const bool inter)
{
	Vector2 hpPos = this->m_hpBar->getPosition() - this->m_hpBar->getPreviousPosition();
	Vector2 panelTransfer = pos - this->m_position;
	if(hpPos != panelTransfer)
		this->m_hpBar->setPosition(pos + (this->m_hpBar->getPosition() - this->m_position), inter);
	Vector2 mpPosTransfer = this->m_manaBar->getPosition() - this->m_manaBar->getPreviousPosition();
	if(mpPosTransfer != panelTransfer)
		this->m_manaBar->setPosition(pos + (this->m_manaBar->getPosition() - this->m_position), inter);
	Vector2 xpPosTransfer = this->m_xpBar->getPosition() - this->m_xpBar->getPreviousPosition();
	if(xpPosTransfer != panelTransfer)
		this->m_xpBar->setPosition(pos + (this->m_xpBar->getPosition() - this->m_position), inter);
	Entity::setPosition(pos, inter);
}
