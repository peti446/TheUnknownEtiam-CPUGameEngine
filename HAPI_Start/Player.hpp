#pragma once
#include "Entity.hpp"
#include <functional>

class GameScene;
class PlayerInfoPanel;
class CursorEntity;
class Text;

enum class ePlayerState
{
	IDLE, WALKING
};

enum class ePlayerOrientation
{
	NORTH = 0, WEST, SOUTH, EAST
};

class Player : public Entity
{
public:
	Player(GameScene& gms, PlayerInfoPanel* pif, CursorEntity& cir, std::string identifier, Rectangle collisionVolum, std::string levelUpSound);
	virtual ~Player();

	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override final;
	virtual void draw(const GRAPHICS::GraphicsSystem& gs, float interV) override final;
	virtual void takeDamage(const float amount) override final;

	void addXP(const int amount);
	int getNextLevelXP() const;
	int getLevel() const;

private:
	//Function to control mana and hp
	void removeMana(float amount);
	void addMana(float amount);
	void addHP(float amount);
	void removeHP(float amount);

	//Cursor reference
	CursorEntity & m_cursorEntity;
	//Player text animation variables
	Text * m_levelUpText;
	DWORD m_lastTextUpdate;
	DWORD m_fadeOutTime;
	//State
	ePlayerState m_state;
	ePlayerOrientation m_orientation;
	//KeyBoard variable
	const HAPI_TKeyboardData& m_keybordData;
	//Stats display
	PlayerInfoPanel * m_statPanel;
	//Level variables 
	int m_currentLevel;
	int m_nextLevelXP;
	int m_currentXP;
	std::string m_levelUpSound;
	//Character stats
	//Stamina determines the max HP of the character
	int m_stamina;
	int m_maxHealth;
	//Intelect defines spell damage and max mana
	int m_intelect;
	int m_maxMana;
	float m_mana;
	//Armor detemrines damage reduction of physical damages
	int m_armor;
	//Function to spawn a spell
	GameScene& m_gameScene;
	//Spell stats
	DWORD m_firstSpellLastTime;
	DWORD m_secondSpellLastTime;
	DWORD m_thirdSpellLastTime;
	Text* m_firstColldownDisplay;
	Text* m_secondColdownDisplay;
	Text* m_thirdColdownDisplay;
};

