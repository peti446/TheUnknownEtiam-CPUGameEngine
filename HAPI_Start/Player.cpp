#include "Player.hpp"
#include "PlayerInfoPanel.hpp"
#include "Text.hpp"
#include "CursorEntity.h"
#include "SoundManager.h"
#include "GameScene.hpp"

Player::Player(GameScene& gms, PlayerInfoPanel* pif, CursorEntity& cir, std::string identifier, Rectangle collisionVolum, std::string levelUpSound) :
																				 Entity(identifier, collisionVolum, 0),
																				 m_cursorEntity(cir),
																				 m_levelUpText(new Text("", Rectangle(0,1,0,1), 34, HAPI_TColour::MAGENTA, HAPI_TextStyle::eBold)),
																				 m_fadeOutTime(5000),
																		         m_lastTextUpdate(0), m_levelUpSound(levelUpSound),
																				 m_state(ePlayerState::IDLE),
																				 m_orientation(ePlayerOrientation::SOUTH),
																				 m_keybordData(HAPI.GetKeyboardData()),
																			     m_statPanel(pif),
																				 m_currentLevel(1),
																				 m_nextLevelXP(50),
																			     m_currentXP(0),
																				 m_gameScene(gms),
																				 m_firstSpellLastTime(HAPI.GetTime()),
																				 m_secondSpellLastTime(HAPI.GetTime()),
																				 m_thirdSpellLastTime(HAPI.GetTime()),
																				 m_firstColldownDisplay(new Text("", Rectangle(0, 1, 0, 1), 21, HAPI_TColour::RED, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Bottom, 10, -70)),
																				 m_secondColdownDisplay(new Text("", Rectangle(0, 1, 0, 1), 21, HAPI_TColour::RED, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Bottom, 10, -50)),
																				 m_thirdColdownDisplay(new Text("", Rectangle(0, 1, 0, 1), 21, HAPI_TColour::RED, HAPI_TextStyle::eBold, Text::eUITextAlignmentHoriz::Left, Text::eUITextAlignmentVert::Bottom, 10, -30))
{
	//Player Stats startup
	//55 stamina ~= 123hp
	//45 intelect ~= 47 DPS and 51 MP
	//25 Armor ~= 1.25% damage reduction
	this->m_stamina = 55;
	this->m_intelect = 45;
	this->m_armor = 25;
	this->m_maxMana = (int)(this->m_intelect * 3.55f);
	this->m_mana = (float)this->m_maxMana;
	this->m_maxHealth = (int)(this->m_stamina * 2.25f);
	this->m_health = (float)this->m_maxHealth;
	//Set the panel Info 
	this->m_statPanel->setHp((int)this->m_health, this->m_maxHealth);
	this->m_statPanel->setMP((int)this->m_mana, this->m_maxMana);
	this->m_statPanel->setXP(this->m_currentXP, this->m_nextLevelXP);
	this->m_statPanel->setCurrentLevel(this->m_currentLevel);

	this->m_speed = 0.528f;

	#if defined(_DEBUG) | defined(DEBUG)
		this->m_speed = 6.f;
	#endif

	this->m_side = eSide::Player;
	this->m_animTime = 250;
}


Player::~Player()
{
	delete this->m_levelUpText;
	delete this->m_firstColldownDisplay;
	delete this->m_secondColdownDisplay;
	delete this->m_thirdColdownDisplay;
}

void Player::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	//Check player alive
	if (!this->isActive())
	{
		//Set the panel healt again so we display  0 and not the value before we are dead;
		this->m_statPanel->setHp((int)this->m_health, this->m_maxHealth);
		//We are dead bring up scoring screen
		this->m_gameScene.playerDied();
		return;
	}

	//By default we are at IDLE if the player does something its get updated
	this->m_state = ePlayerState::IDLE;

	//Level up handler
	{
		int overXP = this->m_currentXP - this->m_nextLevelXP;
		if (overXP >= 0 && this->m_currentLevel < 150)
		{
			//Level up
			float levelModifier = this->m_currentLevel * 1.1f;
			this->m_currentLevel++;
			//Set the remaining xp to the current level, increase next level requirement by a 10% of the current level xp
			this->m_currentXP = overXP;
			this->m_nextLevelXP = (int)((float)this->m_nextLevelXP * 1.10f);
			this->m_lastTextUpdate = HAPI.GetTime();
			this->m_levelUpText->setText("You reached level " + std::to_string(this->m_currentLevel) + "!");
			this->m_levelUpText->setRelativeRect(cameraRect);
			sm.stopSound("levelUp");
			sm.PlaySound("levelUp", this->m_levelUpSound, HAPI_TSoundOptions(0.1f));
			if (this->m_currentLevel == 150)
			{
				this->m_nextLevelXP = 1;
				this->m_currentXP = 1;
			}
			//Ajust stats
			// 1 Stamina = +2.25 MAX HP (Each level grands 5 stamina * level Modifier (1.1f * this->currentLevel)
			// 1 Stamina also gives the user +0.15HP second
			// 1 Intelect = 1.05 Damage (Each level grants 2 intelect * level Modifier (1.1f * this->currentLevel)
			// 1 Intelect also gives the user + 3.55 MAX MP (Rounded down every time of calculation)
			// 1 Intelect also gives the user + 0.25 MP second
			// 1 Armor = 0.05 Damage reduction (Each level grants 2 armor * level Modifier (1.1f * this->currentLevel)
			//Armor is Capped at 25% (500 Armor to avoid not getting damage at all (makes the game easy))
			//Rest scales until max int range (Thats a lot of hp and damage, but enemies will scale too)
			//All stats are rounded down to make it easier to work with
			this->m_stamina += (int)(5.f * levelModifier);
			this->m_intelect += (int)(2.f * levelModifier);
			this->m_armor += (int)(2.f * levelModifier);
			if (this->m_armor > 500)
			{
				this->m_armor = 500;
			}

			//Leveling up also resets hp and mana! (So if you are in fight you benefit from it :) (But ofc you will not be able to exploit it))
			this->m_maxMana = (int)(this->m_intelect * 3.55f);
			this->m_mana = (float)this->m_maxMana;
			this->m_maxHealth = (int)(this->m_stamina * 2.25f);
			this->m_health = (float)this->m_maxHealth;
		}

		//Level up text handling
		DWORD textTime = HAPI.GetTime() - this->m_lastTextUpdate;
		if (textTime < this->m_fadeOutTime)
		{
			float interValue = std::max(0.f, std::min((float)textTime / (float)this->m_fadeOutTime, 1.f));
			HAPI_TColour newColor = this->m_levelUpText->getColor();
			newColor.alpha = (BYTE)(255.f + ((0.f - 255.f) * interValue));
			this->m_levelUpText->setColor(newColor);
			this->m_levelUpText->setHorizontalOffset(-cameraRect.getLeft());
			this->m_levelUpText->setVerticalOffset(-cameraRect.getTop());
		}
	}

	//Movement handling
	{
		Vector2 newPosition(this->m_position);
		if (m_keybordData.scanCode['W'] || m_keybordData.scanCode['S'])
		{
			if (m_keybordData.scanCode['W'])
			{
				newPosition.y -= this->m_speed;
				this->m_orientation = ePlayerOrientation::NORTH;
			}
			if (m_keybordData.scanCode['S'])
			{
				newPosition.y += this->m_speed;
				this->m_orientation = ePlayerOrientation::SOUTH;
			}
			this->m_state = ePlayerState::WALKING;
		}
		else
		{
			if (m_keybordData.scanCode['A'])
			{
				newPosition.x -= this->m_speed;
				this->m_orientation = ePlayerOrientation::WEST;
				this->m_state = ePlayerState::WALKING;
			}
			if (m_keybordData.scanCode['D'])
			{
				newPosition.x += this->m_speed;
				this->m_orientation = ePlayerOrientation::EAST;
				this->m_state = ePlayerState::WALKING;
			}
		}

		//Update animation
		switch (this->m_state)
		{
		case ePlayerState::WALKING:
		{
			this->m_startFrameNum = 112 + ((int)(this->m_orientation) * 14);
			this->m_endFrameNum = 112 + ((int)(this->m_orientation) * 14) + 7;
		}
		break;
		case ePlayerState::IDLE:
		{
			this->m_startFrameNum = 56 + ((int)(this->m_orientation) * 14);
			this->m_endFrameNum = this->m_startFrameNum;
			this->m_framNumber = this->m_startFrameNum;

		}
		break;
		}

		//Make sure we dont get out of the map
		if (newPosition.x < 0 || newPosition.y < 0)
		{
			newPosition.x = std::max(0.0f, newPosition.x);
			newPosition.y = std::max(0.0f, newPosition.y);
		}

		//Set new Position
		this->setPosition(newPosition);
	}

	//Spell handling
	{
		DWORD now = HAPI.GetTime();
		if (m_keybordData.scanCode['1'] && this->m_mana >= (this->m_maxMana*0.15) && this->m_cursorEntity.getSelectedEnemy() != nullptr
			&& now - this->m_firstSpellLastTime >= 1000)
		{
			float d = (this->m_cursorEntity.getSelectedEnemy()->getPosition() - this->m_position).manhattanDistance();
			if (d < 240.f)
			{
				//Spell 1 Launched
				this->m_firstSpellLastTime = now;
				this->removeMana((float)this->m_maxMana*0.15f);
				//Base damage is 1.125f * ucrrent level for the fixed spell
				this->m_gameScene.spawnSpell(((float)this->m_intelect * 1.05f) + (1.125f * this->m_currentLevel), this, this->m_cursorEntity.getSelectedEnemy(), SpellEntity::eSpellType::Follow, Vector2(0, 0), "FireSpell", 500, 0, 5);
			}
		}
		else if (m_keybordData.scanCode['2'] && this->m_mana >= ((float)this->m_maxMana*0.20f) && now - this->m_secondSpellLastTime >= 2500)
		{
			this->m_secondSpellLastTime = now;
			this->removeMana((float)this->m_maxMana*0.20f);

			//Spell 2 Launched
			Vector2 dir = (this->m_cursorEntity.getPosition() - this->m_position);
			dir.NormaliseInPlace();
			//Base damage is 1.35f * ucrrent level for the fixed spell
			this->m_gameScene.spawnSpell(((float)this->m_intelect * 1.05f) + (1.35f * this->m_currentLevel),this, &this->m_cursorEntity, SpellEntity::eSpellType::SkillShot, dir, "NitricSpell", 500, 0, 5);
		}
		else if (m_keybordData.scanCode['3'] && now - this->m_thirdSpellLastTime >= 7500)
		{
			//Spell 3 Launched
			this->m_thirdSpellLastTime = now;
			this->m_currentXP = this->m_nextLevelXP;
		}

		//Update cooldown text
		Rectangle textRect = Rectangle(0, cameraRect.getWidth(), 0, cameraRect.getHeight());
		this->m_firstColldownDisplay->setRelativeRect(textRect);
		float fcoldown = (float)(now - this->m_firstSpellLastTime) - 1000.f;
		fcoldown = -std::min(0.f, fcoldown) + 1.f;
		this->m_firstColldownDisplay->setText("Cooldown Spell 1 : " + std::to_string((int)(fcoldown*0.001f)) + " seconds");
		this->m_secondColdownDisplay->setRelativeRect(textRect);
		float scoldown = (float)(now - this->m_secondSpellLastTime) - 2500.f;
		scoldown = -std::min(0.f, scoldown) + 1.f;
		this->m_secondColdownDisplay->setText("Cooldown Spell 2: " + std::to_string((int)(scoldown*0.001f)) + " seconds");
		this->m_thirdColdownDisplay->setRelativeRect(textRect);
		float tcoldown = (now - this->m_thirdSpellLastTime) - 7500.f;
		tcoldown = -std::min(0.f, tcoldown) + 1.f;
		this->m_thirdColdownDisplay->setText("Cooldown Spell 3 : " + std::to_string((int)(tcoldown*0.001f)) + " seconds");
	}
	//Mana Regen
	//Base mana regen is 0.5 mana second
	//Intelect regen * 0.25f * (tick interval*0.001)
	float scondMultiplier = (this->m_gameScene.getTickInterval() * 0.001f);
	this->addMana((this->m_intelect * 0.25f * scondMultiplier) + (0.5f * scondMultiplier));

	//HP Regen
	//Base HP regen is 0.25 second
	//Stamina regen * 0.15 * (tick interval*0.001)
	this->addHP((this->m_stamina * 0.15f * scondMultiplier) + (0.25f * scondMultiplier));

	//Update panel
	this->m_statPanel->setHp((int)this->m_health, this->m_maxHealth);
	this->m_statPanel->setMP((int)this->m_mana, this->m_maxMana);
	this->m_statPanel->setXP(this->m_currentXP, this->m_nextLevelXP);
	this->m_statPanel->setCurrentLevel(this->m_currentLevel);
}

void Player::draw(const GRAPHICS::GraphicsSystem & gs, float interV)
{
	Entity::draw(gs, interV);
	this->m_statPanel->draw(gs, interV);
	HAPI.RenderText((int)this->m_levelUpText->getTextRelativePosition().x,
		(int)this->m_levelUpText->getTextRelativePosition().y,
		this->m_levelUpText->getColor(), this->m_levelUpText->getText(), this->m_levelUpText->getTextSize(), this->m_levelUpText->getTextStyle());

	HAPI.RenderText((int)this->m_firstColldownDisplay->getTextRelativePosition().x,
		(int)this->m_firstColldownDisplay->getTextRelativePosition().y,
		this->m_firstColldownDisplay->getColor(), this->m_firstColldownDisplay->getText(), this->m_firstColldownDisplay->getTextSize(), this->m_firstColldownDisplay->getTextStyle());

	HAPI.RenderText((int)this->m_secondColdownDisplay->getTextRelativePosition().x,
		(int)this->m_secondColdownDisplay->getTextRelativePosition().y,
		this->m_secondColdownDisplay->getColor(), this->m_secondColdownDisplay->getText(), this->m_secondColdownDisplay->getTextSize(), this->m_secondColdownDisplay->getTextStyle());

	HAPI.RenderText((int)this->m_thirdColdownDisplay->getTextRelativePosition().x,
		(int)this->m_thirdColdownDisplay->getTextRelativePosition().y,
		this->m_thirdColdownDisplay->getColor(), this->m_thirdColdownDisplay->getText(), this->m_thirdColdownDisplay->getTextSize(), this->m_thirdColdownDisplay->getTextStyle());
}

void Player::takeDamage(const float amount)
{
	Entity::takeDamage(amount - (amount * this->m_armor * 0.0005f));
}

void Player::addXP(const int amount)
{
	this->m_currentXP += amount;
}

int Player::getNextLevelXP() const
{
	return this->m_nextLevelXP;
}

int Player::getLevel() const
{
	return this->m_currentLevel;
}

void Player::removeMana(float amount)
{
	this->m_mana -= amount;
	this->m_mana = std::max(0.f, this->m_mana);
}

void Player::addMana(float amount)
{
	this->m_mana += amount;
	this->m_mana = std::min((float)this->m_maxMana, this->m_mana);
}

void Player::addHP(float amount)
{
	this->m_health += amount;
	this->m_health = std::min(this->m_health, (float)this->m_maxHealth);
}

void Player::removeHP(float amount)
{
	this->m_health -= amount;
	this->m_health = std::max(this->m_health, 0.f);
}
