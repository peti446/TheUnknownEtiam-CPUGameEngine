#include "GameScene.hpp"
#include "GameOverScene.hpp"
#include <random>
#include "SceneManager.hpp"
#include "Simulation.hpp"
#include "GraphicsSystem.hpp"
#include "SoundManager.h"
#include "Entity.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "TMap.hpp"
#include "CursorEntity.h"
#include "PlayerInfoPanel.hpp"
#include "EnemyInfoPanel.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::updateScene()
{
	//Camera ajustments
	{
		const Rectangle cameraRect = this->m_sceneM->getSimulation().getGraphicsSystem().getCameraRect();
		Rectangle cameraMoveRect = cameraRect;
		cameraMoveRect.reduceRectangleTo(0.9f, 0.9f);
		const Rectangle playerRect = this->m_PlayerEntity->getColisionVolume();
		if (!cameraMoveRect.CompleatlyContains(playerRect))
		{
			if (playerRect.getLeft() < cameraMoveRect.getLeft())
			{
				int widthModifier = -cameraRect.getWidth() + (cameraRect.getWidth() - cameraMoveRect.getWidth() + this->m_currentMap->getTileWidth());
				if (widthModifier + cameraRect.getLeft() >= 0)
					this->m_sceneM->getSimulation().getGraphicsSystem().translateCamera(widthModifier, 0);
			}
			else if (playerRect.getLeft() > cameraMoveRect.getRight())
			{
				this->m_sceneM->getSimulation().getGraphicsSystem().translateCamera(cameraRect.getWidth() - (cameraRect.getWidth() - cameraMoveRect.getWidth() + this->m_currentMap->getTileWidth()), 0);
			}
			else if (playerRect.getTop() < cameraMoveRect.getTop())
			{
				int heightModifier = -cameraRect.getHeight() + (cameraRect.getHeight() - cameraMoveRect.getHeight() + this->m_currentMap->getTileHeight());
				if (heightModifier + cameraRect.getTop() >= 0)
					this->m_sceneM->getSimulation().getGraphicsSystem().translateCamera(0, heightModifier);
			}
			else if (playerRect.getTop() > cameraMoveRect.getBottom())
			{
				this->m_sceneM->getSimulation().getGraphicsSystem().translateCamera(0, cameraRect.getHeight() - (cameraRect.getHeight() - cameraMoveRect.getHeight() + this->m_currentMap->getTileHeight()));
			}
		}
	}

	//Update entities
	Layer& player_layer = this->m_currentMap->getPlayerLayer();
	std::vector<Layer>& map = this->m_currentMap->getMap();


	std::vector<Entity*>::iterator entitiVetorIterator;

	for (Layer& layer : map)
	{
		int size = (int)layer.m_tiles.size();
		for (int i = 0; i < size; i++)
		{
			std::vector<Entity*>& entities = layer.m_tiles[i];
			for (entitiVetorIterator = entities.begin(); entitiVetorIterator != entities.end();)
			{
				if (((*entitiVetorIterator)->getSide() == Entity::eSide::Enemy || (*entitiVetorIterator)->getSide() == Entity::eSide::Spell) && !(*entitiVetorIterator)->isActive())
				{
					if ((*entitiVetorIterator)->getSide() == Entity::eSide::Enemy)
					{
						this->m_deadEnemies.push_back((*entitiVetorIterator));
					}
					else
					{
						this->m_spells.push_back((*entitiVetorIterator));
					}
					entitiVetorIterator = entities.erase(entitiVetorIterator);
					continue;
				}
				(*entitiVetorIterator)->update(this->m_sceneM->getSimulation().getGraphicsSystem().getCameraRect(), this->m_sceneM->getSimulation().getSoundManager());
				Vector2 pos = (*entitiVetorIterator)->getPosition();
				int newX = (int)pos.x / this->m_currentMap->getTileWidth();
				int newY = (int)pos.y / this->m_currentMap->getTileHeight();
				int currentX = i % this->m_currentMap->getMapWidth();
				int currentY = i / this->m_currentMap->getMapWidth();

				//Out of bound checking
				if (newX >= (int)this->m_currentMap->getMapWidth() - 1|| newY >= (int)this->m_currentMap->getMapHeight() - 1)
				{
					newX = std::min(newX, (int)this->m_currentMap->getMapWidth());
					newY = std::min(newY, (int)this->m_currentMap->getMapHeight());
					(*entitiVetorIterator)->setPosition(newX * this->m_currentMap->getTileWidth(), newY * this->m_currentMap->getTileHeight(), false);
				}
				else if (newX < 0 || newY < 0)
				{
					newX = std::max(0, newX);
					newY = std::max(0, newY);
				}

				if (newX != currentX || newY != currentY)
				{
					this->m_entitiesToMove.push(std::make_pair(newX + (newY * this->m_currentMap->getMapWidth()), (*entitiVetorIterator)));
					entitiVetorIterator = entities.erase(entitiVetorIterator);
				}
				else
				{
					entitiVetorIterator++;
				}
			}
		}
		//Move entities
		while (this->m_entitiesToMove.size() != 0)
		{
			std::pair<int, Entity*>& pair = this->m_entitiesToMove.front();
			layer.m_tiles[pair.first].push_back(pair.second);
			this->m_entitiesToMove.pop();
		}
	}

	//Collision
	for (unsigned int y = 0; y < this->m_currentMap->getMapHeight(); y++)
	{
		for (unsigned int x = 0; x < this->m_currentMap->getMapWidth(); x++)
		{
			std::vector<Entity*>& currentTileentity = player_layer.m_tiles[x + (y*this->m_currentMap->getMapWidth())];
			for (Entity* e : currentTileentity)
			{
				for (Entity* e2 : currentTileentity)
				{
					if(e != e2)
						if (e->isColliding(*e2))
						{
							e->collision(*e2);
						}
				}
			}
			if (y < this->m_currentMap->getMapHeight()-1)
			{
				std::vector<Entity*>& nextYTile = player_layer.m_tiles[x + ((y + 1)*this->m_currentMap->getMapWidth())];
				for (Entity* me : currentTileentity)
				{
					for (Entity* nye : nextYTile)
					{
						if (me->isColliding(*nye))
						{
							me->collision(*nye);
						}
					}
				}
			}
			if (x < this->m_currentMap->getMapWidth() - 1)
			{
				std::vector<Entity*>& nextXEntity = player_layer.m_tiles[(x + 1) + (y*this->m_currentMap->getMapWidth())];
				for (Entity* me : currentTileentity)
				{
					for (Entity* nxe : nextXEntity)
					{
						if (me->isColliding(*nxe))
						{
							me->collision(*nxe);
						}
					}
				}
			}
		}
	}

	//Update ui
	for (Entity* uie : this->m_entities)
	{
		//Update
		uie->update(this->m_sceneM->getSimulation().getGraphicsSystem().getCameraRect(), this->m_sceneM->getSimulation().getSoundManager());
		//As ui elements cannot colide agenst each other just check collision of each ui element agains the cursor
		if (this->m_cursorEntity->isColliding(*uie))
		{
			this->m_cursorEntity->collision(*uie);
		}
	}

	//Spawn more enemies every 30 seconds if possible
	if (this->m_deadEnemies.size() < 0 &&  HAPI.GetTime() - this->m_lastTimeSpawned >= 30000)
		return;
	this->m_lastTimeSpawned = HAPI.GetTime();
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<size_t> uid(0, this->m_deadEnemies.size() - 1);
	std::uniform_int_distribution<int> levelModifier(0, 2);
	Player* p = (Player*)this->m_PlayerEntity;
	std::uniform_int_distribution<int> level(std::max((int)(p->getLevel() - levelModifier(dre)), 1) , p->getLevel() + levelModifier(dre));
	size_t toSpawn = uid(dre);
	for (int i = 0; i < toSpawn; i++)
	{
		this->spawnRandomEnemy(level(dre));
	}
}

void GameScene::drawScene(float interV)
{
	GRAPHICS::GraphicsSystem& gs = this->m_sceneM->getSimulation().getGraphicsSystem();
	gs.ClearScreen();
	Vector2 cameraPos = gs.getCameraPosition();

	const int startingRow = (int)(cameraPos.y / this->m_currentMap->getTileHeight());
	const int startingColumn = (int)(cameraPos.x / this->m_currentMap->getTileWidth());
	const int maxRow = std::min(startingRow + this->m_rowsToDraw, (int)this->m_currentMap->getMapHeight());
	const int maxColumn = std::min(startingColumn + this->m_columnsToDraw, (int)this->m_currentMap->getMapHeight());

	std::vector<Layer>& mapToDraw = this->m_currentMap->getMap();

	for (Layer& layer : mapToDraw)
	{
		for (int y = startingRow; y < maxRow; y++)
		{
			for (int x = startingColumn; x < maxColumn; x++)
			{
				for (Entity* e : layer.m_tiles[x + (y*this->m_currentMap->getMapWidth())])
				{
					e->draw(gs, interV);
				}
			}
		}

	}
	
	for (Entity* e : this->m_entities)
	{
		e->draw(gs, interV);
	}

	//Draw entity at the end
	this->m_cursorEntity->draw(gs, interV);
}

void GameScene::loadScene(SceneManager& sm)
{
	if (this->m_isLoaded)
		return;

	//Set Spawn time
	this->m_lastTimeSpawned = HAPI.GetTime();

	GRAPHICS::GraphicsSystem& gs = sm.getSimulation().getGraphicsSystem();
	SOUND::SoundManager& soundmanager = sm.getSimulation().getSoundManager();
	gs.LoadTexture("ExteriorTilesSheet", "Data/Tiles/exteriorTiles.png", "Data/Tiles/exteriorTiles.xml");

	this->m_currentMap = new TMap();
	if (!this->m_currentMap->loadMap("Data/Maps/map_00001.tmx", gs))
	{
		std::cerr << "Could not load the map " << "Data/Maps/map_00001.tmx" << std::endl;
	}
	
	//Precalculate rows and columns
	this->m_rowsToDraw = (gs.getHeight() / this->m_currentMap->getTileHeight()) + 1;
	this->m_columnsToDraw = (gs.getWidth() / this->m_currentMap->getTileWidth()) + 1;

	//Load textures
	gs.LoadTexture("SelectedRing", "Data/Characters/Selected Circle.png");
	gs.LoadTexture("Player", "Data/Characters/roguelikeChar.png", "Data/Characters/roguelikeChar.xml");


	//Set up common UI
	PlayerInfoPanel* pif = new PlayerInfoPanel(gs);
	this->m_entities.push_back(pif);

	EnemyInfoPanel* eip = new EnemyInfoPanel(gs, Vector2(pif->getColisionVolume().getWidth() + pif->getPosition().x, pif->getPosition().y));
	this->m_entities.push_back(eip);

	gs.LoadTexture("CursorNormal", "Data/UI/cursorGauntlet_bronze.png");
	CursorEntity* ce = new CursorEntity(eip, "CursorNormal", gs.getRect("CursorNormal"));
	this->m_cursorEntity = ce;
	this->m_currentMap->getPlayerLayer().m_tiles[1].push_back(ce);

	//Add the player
	soundmanager.LoadSound("Data/Sounds/levelUp.wav");
	gs.LoadTexture("FireSpell", "Data/Spells/fireBallSpriteSheet.png", "Data/Spells/fireBallSpriteSheet.xml");
	gs.LoadTexture("NitricSpell", "Data/Spells/NitricBallSpriteSheet.png", "Data/Spells/NitricBallSpriteSheet.xml");
	gs.LoadTexture("PlayerAnimSpritesheet", "Data/Characters/player.png", "Data/Characters/player.xml");
	Player* p = new Player(*this, pif, *ce,"PlayerAnimSpritesheet", gs.getRect("Player"), "Data/Sounds/levelUp.wav");
	p->setPosition(26 * this->m_currentMap->getTileWidth(), 31 * this->m_currentMap->getTileHeight(), false);
	this->m_PlayerEntity = p;
	this->m_currentMap->getPlayerLayer().m_tiles[26 + (31 * this->m_currentMap->getMapWidth())].push_back(p);


	//Enemy spawn points, could make them random, but to find enemies quicker will spawn at all locations
	std::vector<Rectangle> spawnZones = {
		Rectangle(73,98, 1,32),
		Rectangle(18,61,41,54),
		Rectangle(76,94,35, 70),
		Rectangle(17,41,56,71),
		Rectangle(35, 68, 49, 66),
		Rectangle(64, 96, 78, 96),
		Rectangle(1,54,72,97) 
	};

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <float> urd(0.2f, 1.f);

	//Load Enemies
	for (Rectangle r : spawnZones)
	{
		int numberOfEnemiesInThisSpawnZone = std::max((int)(0.01f*(r.getWidth() * r.getHeight()) * urd(dre)), 1);
		for (int i = 0; i < numberOfEnemiesInThisSpawnZone; i++)
		{
			this->m_deadEnemies.push_back(new Enemy(this->m_PlayerEntity, *this, *this->m_currentMap, r, "Player", "SelectedRing", gs.getRect("Player")));
		}
	}
	//Suffle vector for randomness when spawming in the map
	std::random_shuffle(this->m_deadEnemies.begin(), this->m_deadEnemies.end());

	//Load spells
	soundmanager.LoadSound("Data/Sounds/FireBall.ogg");
	this->m_spells.reserve(150);
	for (int i = 0; i < this->m_spells.capacity(); i++)
	{
		this->m_spells.push_back(new SpellEntity());
	}


	soundmanager.LoadSound("Data/Sounds/yesterbreeze.ogg");
	HAPI_TSoundOptions scfg = HAPI_TSoundOptions(0.1f, true);
	soundmanager.PlaySound("Map01SoundLoop", "Data/Sounds/yesterbreeze.ogg", scfg);

	//create Game Over scene
	GameOverScene* gameOver = new GameOverScene();
	gameOver->loadScene(sm);
	gameOver->setParent(this);
	sm.storeScene("GameOverScene",*gameOver);
	this->m_sceneM = &sm;

	this->m_isLoaded = true;
}

void GameScene::unloadSceneData()
{
	if (!this->m_isLoaded)
		return;
	
	delete this->m_currentMap;

	for (Entity* e : this->m_entities)
		delete e;
	
	for (Entity* e : this->m_deadEnemies)
		delete e;
	
	for (Entity* e : this->m_spells)
		delete e;

	this->m_isLoaded = false;
}

void GameScene::spawnSpell(const float damage, const Entity* const owner, const Entity* const targe, const SpellEntity::eSpellType type, const Vector2 dir, const std::string identifier, const DWORD frameTimeMS, const int frameNumStart, const int frameNumEnd)
{
	if(this->m_spells.size() < 0)
		return;
	SpellEntity* se = (SpellEntity*)this->m_spells.back();
	this->m_spells.pop_back();
	se->Instanciate(damage,owner, targe, type, dir, identifier, this->m_sceneM->getSimulation().getGraphicsSystem().getRect(identifier), frameTimeMS, frameNumStart, frameNumEnd);
	Vector2 pos = se->getPosition();
	pos.x = pos.x / (float)this->m_currentMap->getTileWidth();
	pos.y= pos.y / (float)this->m_currentMap->getTileHeight();
	this->m_entitiesToMove.push(std::make_pair((int)(pos.x + (pos.y * this->m_currentMap->getMapWidth())), (Entity*)se));
}

void GameScene::spawnRandomEnemy(const int level)
{
	if (this->m_deadEnemies.size() < 0)
		return;
	Enemy* e = (Enemy*)this->m_deadEnemies.back();
	this->m_deadEnemies.pop_back();
	e->Spawn(level);
	int x = (int)(e->getPosition().x / this->m_currentMap->getTileWidth());
	int y = (int)(e->getPosition().y / this->m_currentMap->getTileHeight());
	this->m_currentMap->getPlayerLayer().m_tiles[x + (y * this->m_currentMap->getMapWidth())].push_back(e);
}

const DWORD GameScene::getTickInterval() const
{
	return this->m_sceneM->getSimulation().getTickInterval();
}

void GameScene::playerDied()
{
	this->m_sceneM->changeScene(*this->m_sceneM->retriveStoredScene("GameOverScene"), false);
	this->m_sceneM->storeScene("GameScene", *this);
	//Stop sounds
	this->m_sceneM->getSimulation().getSoundManager().StopAllSounds();
	//Hide the cursor :D
	this->m_cursorEntity->setActive(false);
}
