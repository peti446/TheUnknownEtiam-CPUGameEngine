#include "Enemy.hpp"
#include "TMap.hpp"
#include <random>
#include "GraphicsSystem.hpp"
#include "SoundManager.h"
#include "Player.hpp"
#include "SpellEntity.hpp"
#include "GameScene.hpp"

Enemy::Enemy(const Entity* const player, GameScene& gameScene, TMap& map, Rectangle spawnArea, std::string identifier, std::string selectedIdentifier, Rectangle collisionVolumen, int frameNum) :
																							Entity(identifier, collisionVolumen, frameNum),
																							m_player(player),
																							m_gameScene(gameScene),
																							m_lastAtacked(HAPI.GetTime()),
																							m_selected(false),
																							m_selectedIdentifier(selectedIdentifier),
																							m_currentLevel(1),
																							m_maxHealth(100),
																							m_maxMana(100),
																							m_mana(100),
																							m_mapForPath(map),
																							m_spawnArea(spawnArea),
																							m_xpReward(10)
{
	this->m_active = false;
	//Set speed and hp
	this->m_speed = 0.528f;
	this->m_health = 100;
	//Create path nodes
	int pathNodesToCreate = (spawnArea.getHeight() * spawnArea.getWidth());
	this->m_side = eSide::Enemy;
	for (size_t i = 0; i < pathNodesToCreate; i++)
	{
		this->m_freeNodesToUse.push(new PathNode());
	}

	//Set up initial stats
	//65 stamina 
	//10 intelect
	//10 Armor
	this->m_stamina = 65;
	this->m_intelect = 15;
	this->m_armor = 10;
	this->m_maxMana = (int)(this->m_intelect * 3.55f);
	this->m_mana = (float)this->m_maxMana;
	this->m_maxHealth = (int)(this->m_stamina * 2.25f);
	this->m_health = (float)this->m_maxHealth;
}

Enemy::~Enemy()
{
	while (!this->m_path.empty())
	{
		delete this->m_path.top();
		this->m_path.pop();
	}

	while (!this->m_freeNodesToUse.empty())
	{
		delete this->m_freeNodesToUse.front();
		this->m_freeNodesToUse.pop();
	}
}

void Enemy::collision(Entity & other)
{
	Entity::collision(other);
	//Find new path
	findNewPath();
}

void Enemy::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;

	//If player is in range atack him!
	float d = (this->m_player->getPosition() - this->m_position).manhattanDistance();
	if (d < 140.f)
	{
		DWORD now = HAPI.GetTime();
		if (this->m_mana >= (this->m_maxMana*0.15) && now - this->m_lastAtacked >= 1500)
		{
			{
				//Spell 1 Launched
				this->m_lastAtacked = now;
				this->removeMana((float)this->m_maxMana*0.15f);
				//Base damage is 1.125f * ucrrent level for the fixed spell
				this->m_gameScene.spawnSpell(((float)this->m_intelect * 1.05f) + (1.125f * this->m_currentLevel), this, this->m_player, SpellEntity::eSpellType::Follow, Vector2(0, 0), "FireSpell", 500, 0, 5);
			}
		}
	}
	else if (!this->m_path.empty())
	{
		Vector2 newPos = this->m_position;

		PathNode* pn = this->m_path.top();

		int xTile = pn->getxPos();
		int yTile = pn->getyPos();

		int xCurrent = (int)(newPos.x / this->m_mapForPath.getTileWidth());
		int yCurrent = (int)(newPos.y / this->m_mapForPath.getTileHeight());

		int xDir = xTile - xCurrent;
		int yDir = yTile - yCurrent;

		newPos.x += (float)xDir * this->m_speed;
		newPos.y += (float)yDir * this->m_speed;

		xCurrent = (int)(newPos.x / this->m_mapForPath.getTileWidth());
		yCurrent = (int)(newPos.y / this->m_mapForPath.getTileHeight());

		if (xCurrent == xTile && yCurrent == yTile)
		{
			this->m_freeNodesToUse.push(pn);
			this->m_path.pop();
		}

		this->setPosition(newPos);
	}
	else
	{
		this->findNewPath();
	}
	//Enemies will not have HP regen
	//Mana Regen
	//Base mana regen is 0.5 mana second
	//Intelect regen * 0.25f * (tick interval*0.001)
	float scondMultiplier = (this->m_gameScene.getTickInterval() * 0.001f);
	this->addMana((this->m_intelect * 0.25f * scondMultiplier) + (0.5f * scondMultiplier));
}

void Enemy::draw(const GRAPHICS::GraphicsSystem & gs, float interV)
{
	Entity::draw(gs, interV);
	
	if (!this->m_active || this->m_framNumber < 0)
		return;

	Vector2 interPos = this->getPreviousPosition() + ((this->m_position - this->getPreviousPosition()) * interV);

	if (this->m_selected)
	{
		gs.Draw(this->m_selectedIdentifier, interPos);
	}
}

void Enemy::takeDamage(const float amount)
{
	Entity::takeDamage(amount - (amount * this->m_armor * 0.0005f));
}

void Enemy::setSelected(bool selected)
{
	this->m_selected = selected;
}

void Enemy::Spawn(const int level)
{	
	//Set active
	this->m_active = true;
	//Set selected
	this->m_selected = false;
	//Set stats based on level
	this->m_currentLevel = level;
	//Set XP reward, give around 20% of xp per enemy killed plus 2.5% extra for each level higer the enemy is
	const Player* const p = (Player*)this->m_player;
	this->m_xpReward =  (int)((0.2f * p->getNextLevelXP()) + ((0.025f * std::max((level - p->getLevel()), 0)) * p->getNextLevelXP()));
	//Set the scaling of the stats
	//Ajust stats
	// 1 Stamina = +2.25 MAX HP (Each level grands 6 stamina * level Modifier (level * 1.1f))
	// 1 Stamina also gives the user +0.15HP second
	// 1 Intelect = 1.05 Damage (Each level grants 1 intelect * level Modifier (level * 1.1f))
	// 1 Intelect also gives the user + 3.55 MAX MP (Rounded down every time of calculation)
	// 1 Intelect also gives the user + 0.25 MP second
	// 1 Armor = 0.05% Damage reduction (Each level grants 1 armor * level Modifier (level * 1.1f))
	//Armor is Capped at 25% (500 Armor to avoid not getting damage at all (makes the game easy))
	//Rest scales until max int range (Thats a lot of hp and damage, but enemies will scale too)
	//All stats are rounded down to make it easier to work with
	float levelModifier = level * 1.1f;
	this->m_stamina += (int)(6.f * levelModifier);
	this->m_intelect += (int)(1.f * levelModifier);
	this->m_armor += (int)(1.f * levelModifier);
	if (this->m_armor > 500)
	{
		this->m_armor = 500;
	}

	this->m_maxMana = (int)(this->m_intelect * 3.55f);
	this->m_mana = (float)this->m_maxMana;
	this->m_maxHealth = (int)(this->m_stamina * 2.25f);
	this->m_health = (float)this->m_maxHealth;



	//Set initial position and a path
	std::random_device rd;
	std::default_random_engine drw(HAPI.GetTime());
	std::uniform_int_distribution<int> uid(0, this->m_spawnArea.getWidth()-1);
	std::uniform_int_distribution<int> uid2(0, this->m_spawnArea.getHeight()-1);
	int spawnPosX = 0;
	int spawnPosY = 0;
	const std::vector<std::vector<Entity*>>& playerLayer = this->m_mapForPath.getPlayerLayer().m_tiles;
	while (true)
	{
		spawnPosX = uid(drw) + this->m_spawnArea.getLeft();
		spawnPosY = uid2(drw) + this->m_spawnArea.getTop();
		this->m_position = Vector2(spawnPosX * (float)this->m_mapForPath.getTileWidth(), spawnPosY * (float)this->m_mapForPath.getTileHeight());
		std::vector<Entity*> tile = playerLayer[spawnPosX + (spawnPosY + this->m_mapForPath.getMapWidth())];
		for (Entity* e : tile)
		{
			if (this->isColliding(*e))
				continue;
		}
		break;
	}

	//Find a paths to go to
	this->findNewPath();

}

int Enemy::getLevel() const
{
	return this->m_currentLevel;
}

int Enemy::getMaxHP() const
{
	return this->m_maxHealth;
}

int Enemy::getMaxMP() const
{
	return this->m_maxMana;
}

int Enemy::getMP() const
{
	return (int)this->m_mana;
}

int Enemy::getHP() const
{
	return (int)this->m_health;
}

int Enemy::getXPReward() const
{
	return this->m_xpReward;
}

void Enemy::removeMana(float amount)
{
	this->m_mana -= amount;
	this->m_mana = std::max(0.f, this->m_mana);
}

void Enemy::addMana(float amount)
{
	this->m_mana += amount;
	this->m_mana = std::min((float)this->m_maxMana, this->m_mana);
}

void Enemy::findNewPath()
{
	//Spawn area info for easy access
	const int SpawnAreaWidth = this->m_spawnArea.getWidth();
	const int SpawnAreaHeight = this->m_spawnArea.getHeight();
	const int SpawnStartX = this->m_spawnArea.getLeft();
	const int SpawnStartY = this->m_spawnArea.getTop();

	//Start and target location local space in the spawn area (Need to be converted to global space later on)
	const int startX = std::max(0, (int)(this->m_position.x / this->m_mapForPath.getTileWidth()) - SpawnStartX);
	const int startY = std::max(0, (int)(this->m_position.y / this->m_mapForPath.getTileHeight()) - SpawnStartY);
	int targetX = 0;
	int targetY = 0;
	{
		//Generate new target pos
		std::default_random_engine e(HAPI.GetTime());
		//X Random
		std::uniform_int_distribution<int> randomX(0, SpawnAreaWidth-1);
		//Y Random
		std::uniform_int_distribution<int> randomY(0, SpawnAreaHeight-1);
		do
		{
			targetX = randomX(e);
			targetY = randomY(e);
		} while (targetX == startX && targetY == startY);
	}

	//Remove any node from the currecnt path if there is still one
	while (this->m_path.size() > 0)
	{
		this->m_freeNodesToUse.push(this->m_path.top());
		this->m_path.pop();
	}

	//Map info
	const std::vector<std::vector<Entity*>>& playerLayer = this->m_mapForPath.getPlayerLayer().m_tiles;
	int dirX[4]{ 1,0,-1,0 };
	int dirY[4]{ 0,1,0,-1 };

	//Structure for the priority queue
	struct Comparator
	{
		bool operator()(const PathNode* a, const PathNode* b) const
		{
			return a->getPriority() > b->getPriority();
		}
	};

	//Lits for the current sets
	std::priority_queue<PathNode*, std::vector<PathNode*>, Comparator> openSet;
	std::vector<int> priorities;
	std::vector<PathNode*> closedSet;
	priorities.resize(SpawnAreaHeight * SpawnAreaWidth, -1);
	closedSet.resize(SpawnAreaHeight * SpawnAreaWidth, nullptr);
	//Start position element
	{
		PathNode* startNode = this->m_freeNodesToUse.front();
		this->m_freeNodesToUse.pop();
		startNode->Initialize(nullptr ,startX, startY, 0, 0);
		startNode->updatePriority(targetX, targetY);
		openSet.push(startNode);
		priorities[startX + (startY * SpawnAreaWidth)] = startNode->getPriority();
	}

	//Start finding the path
	while (!openSet.empty())
	{
		PathNode* foq = openSet.top();
		openSet.pop();

		//Get data from the node
		int foqX = foq->getxPos();
		int foqY = foq->getyPos();
		int foqArrayIndex = foqX + (foqY * SpawnAreaWidth);
		//Mark it as closed, we alraeady visited it
		closedSet[foqArrayIndex] = foq;


		//We reached the targert, so we found a path
		if (foqX == targetX && foqY == targetY)
		{
			//Add the path to the stack, so we can use it in the update
			do
			{
				closedSet[foq->getxPos() + (foq->getyPos() * SpawnAreaWidth)] = nullptr;
				foq->translate(SpawnStartX, SpawnStartY);
				this->m_path.push(foq);
				foq = foq->getParent();
			} while (foq != nullptr);

			//Break the loop as we found the path
			break;
		}

		//Generate frontiers (4 frontiers (UP, RIGHT, DOWN, LEFT), no diagonal movementa allowed)
		for (size_t i = 0; i < 4; i++)
		{
			//Generate new positions
			int newX = foqX + dirX[i];
			int newY = foqY + dirY[i];
			int newNodeArrayIndex = newX + (newY * SpawnAreaWidth);
			//Check if positions are valid
			if (!(newX < 0 || newX > (SpawnAreaWidth-1) || newY < 0 || newY > (SpawnAreaHeight-1)))
			{
				//Check if we havent visited already, if so ignore.
				if (closedSet[newNodeArrayIndex] != nullptr)
					continue;
				
				bool collided = false;
				{
					const std::vector<Entity*> tile = playerLayer[(newX + SpawnStartX) + ((newY + SpawnStartY) * this->m_mapForPath.getMapWidth())];
					for (Entity* const e : tile)
					{
						if (this->isColliding(*e))
						{
							collided = true;
						}
					}
				}
				if (collided)
					continue;

				//Create a new node
				PathNode* newNode = this->m_freeNodesToUse.front();
				this->m_freeNodesToUse.pop();
				newNode->Initialize(foq, newX, newY, foq->getLevel(), foq->getPriority());
				newNode->nextLevel();
				newNode->updatePriority(targetX, targetY);

				//Check if has a priority, if it has one it means that its still in the open set, and the new node might be more efficient.
				if (priorities[newNodeArrayIndex] == -1)
				{
					priorities[newNodeArrayIndex] = newNode->getPriority();
					openSet.push(newNode);
				}
				else if (priorities[newNodeArrayIndex] > newNode->getPriority())
				{
					//Its in open set list
					//Go trought all nodes to find the node at the newNode position
					std::queue<PathNode*> nodes;
					while(!(openSet.top()->getxPos() == newX && openSet.top()->getyPos() == newY))
					{
						nodes.push(openSet.top());
						openSet.pop();
					}

					this->m_freeNodesToUse.push(openSet.top());
					openSet.pop();

					//Add all the nodes again to the open set
					while (!nodes.empty())
					{
						openSet.push(nodes.front());
						nodes.pop();
					}

					//Add the updated node info
					priorities[newNodeArrayIndex] = newNode->getPriority();
					closedSet[newNodeArrayIndex] = nullptr;
					openSet.push(newNode);
				}
				else
				{
					//Node in open set is more efficient so dont change anything, add the node to the list of precrreated node
					this->m_freeNodesToUse.push(newNode);
				}
			}
		}
	}

	//Finaly garbage colection, so no node is lost (no memory leaks!)
	for (PathNode* p : closedSet)
	{
		if (p != nullptr)
		{
			this->m_freeNodesToUse.push(p);
		}
	}

	//Open Set clean up
	while (!openSet.empty())
	{
		this->m_freeNodesToUse.push(openSet.top());
		openSet.pop();
	}
}
