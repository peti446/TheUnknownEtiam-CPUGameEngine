#pragma once
#include "Entity.hpp"
#include <queue>
#include <stack>

class TMap;
class GameScene;

class PathNode
{
public:

	PathNode() = default;

	void Initialize(PathNode* cameFrom, int xp, int yp, int d, int p)
	{
		parent = cameFrom;  xPos = xp; yPos = yp; level = d; priority = p;
	}

	const int getxPos() const { return xPos; }
	const int getyPos() const { return yPos; }
	const int getLevel() const { return level; }
	const int getPriority() const { return priority; }
	PathNode* const getParent() const { return parent; }

	void updateParent(PathNode* p)
	{
		parent = p;
	}

	void translate(int x, int y)
	{
		xPos += x;
		yPos += y;
	}

	void updatePriority(const int & xDest, const int & yDest)
	{
		priority = level + estimate(xDest, yDest) * 10; //A*
	}

	void nextLevel()
	{
		level += 10;
	}

	// Estimation function for the remaining distance to the goal.
	const int estimate(const int & xDest, const int & yDest) const
	{
		int xd = xDest - xPos;
		int yd = yDest - yPos;

		// Manhattan distance
		return std::abs(xd) + std::abs(yd);
	}
private:
	// current position
	int xPos{ 0 };
	int yPos{ 0 };
	// total distance already travelled to reach the node
	int level{ 0 };
	// priority=level+remaining distance estimate
	int priority{ 0 };  // smaller: higher priority
	PathNode* parent{ nullptr };
};

class Enemy :
	public Entity
{
public:
	Enemy(const Entity* const player, GameScene& gameScene, TMap& map, Rectangle spawnArea, std::string identifier, std::string selectedIdentifier, Rectangle collisionVolumen, int frameNum = 0);
	virtual ~Enemy();

	virtual void collision(Entity& other) override final;
	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override final;
	virtual void draw(const GRAPHICS::GraphicsSystem& gs, float interV) override final;
	virtual void takeDamage(const float amount) override final;

	void setSelected(bool selected);

	void Spawn(const int level);
	
	int getLevel() const;
	int getMaxHP() const;
	int getMaxMP() const;
	int getMP() const;
	int getHP() const;
	int getXPReward() const;

private:
	//Private
	void removeMana(float amount);
	void addMana(float amount);
	//Player info
	const Entity * const m_player;
	//Atack info
	DWORD m_lastAtacked;
	GameScene& m_gameScene;
	//Variables for enemy
	bool m_selected;
	std::string m_selectedIdentifier;
	//Enemy Stats
	int m_currentLevel;
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
	//Reward
	int m_xpReward;

	//Variables for A* Path finding algorithm
	void findNewPath();
	TMap& m_mapForPath;
	Rectangle m_spawnArea;
	std::stack<PathNode*> m_path;
	std::queue<PathNode*> m_freeNodesToUse;
};


