#include "GameOverScene.hpp"
#include "Button.hpp"
#include "SceneManager.hpp"
#include "Simulation.hpp"
#include "GraphicsSystem.hpp"
#include "CursorEntity.h"
#include "UIElement.hpp"

GameOverScene::GameOverScene()
{
}


GameOverScene::~GameOverScene()
{
}

void GameOverScene::updateScene()
{
	//Update the entities, in this case only the UI elements got somthing to update!
	for (int i = 0; i < this->m_entities.size(); i++)
	{
		this->m_entities[i]->update(this->m_sceneM->getSimulation().getGraphicsSystem().getCameraRect(), this->m_sceneM->getSimulation().getSoundManager());
	}
	Entity* cursorentity = this->m_entities.back();
	for (size_t i = 0; i < this->m_entities.size() - 1; i++)
	{
		if (cursorentity->isColliding(*this->m_entities[i]))
		{
			cursorentity->collision(*this->m_entities[i]);
		}
	}

	if (this->m_deleting)
	{
		BaseScene* b = this->m_sceneM->retriveStoredScene("MainMenu");
		this->m_sceneM->changeScene(*b, true);
		BaseScene* gameScene = this->m_sceneM->retriveStoredScene("GameScene");
		if (gameScene != nullptr)
		{
			gameScene->unloadSceneData();
			delete gameScene;
		}
		delete this;
		return;
	}
}

void GameOverScene::drawScene(float interV)
{
	//Draw parents entities
	this->m_parent->drawScene(interV);
	//Draw every entity
	for (Entity* e : this->m_entities)
	{
		e->draw(this->m_sceneM->getSimulation().getGraphicsSystem(), interV);
	}
}

void GameOverScene::loadScene(SceneManager & sm)
{
	//Only load scene if not already loaded
	if (this->m_isLoaded)
	{
		return;
	}

	GRAPHICS::GraphicsSystem& gs = sm.getSimulation().getGraphicsSystem();

	//Loading of buttons into memory
	gs.LoadTexture("ButtomWoodBlankNormal", "Data/UI/panel_woodPaperWear.png");
	gs.resizeTexture("ButtomWoodBlankNormal", 250, 50);
	gs.LoadTexture("ButtonWoodBlankHover", "Data/UI/panel_woodPaperDetail.png");
	gs.resizeTexture("ButtonWoodBlankHover", 250, 50);
	gs.LoadTexture("ButtonWoodBlankPressed", "Data/UI/panel_woodDetail.png");
	gs.resizeTexture("ButtonWoodBlankPressed", 250, 50);

	//Background instanciation
	this->m_entities.push_back(new UIElement(gs.LoadTexture("BackgroundGameOver", "Data/Background/GameMenuOverlay.png"), gs.getRect("BackgroundGameOver")));

	//Title banner
	gs.LoadTexture("TitleBannerGameOver", "Data/UI/bannerModern.png");
	gs.resizeTexture("TitleBannerGameOver", 375, 100);
	UIElement* banner = new UIElement("TitleBanner", gs.getRect("TitleBanner"));
	banner->setPosition(453, 100, false);
	banner->addText("You are dead!", HAPI_TColour::BLACK, 22, HAPI_TextStyle::eBold);
	this->m_entities.push_back(banner);

	//-Play button-
	Button* continueB = new Button("ButtomWoodBlankNormal", "ButtonWoodBlankHover", "ButtonWoodBlankPressed", gs.getRect("ButtomWoodBlankNormal"));
	continueB->connectEvent(UIElement::UIEvents::onRelease, &GameOverScene::MainMenuClicked, this);
	continueB->setPosition(515, 250, false);
	continueB->addText("Go Back to Main Menu!");
	this->m_entities.push_back(continueB);

	//-Exit button-
	Button* exit = new Button("ButtomWoodBlankNormal", "ButtonWoodBlankHover", "ButtonWoodBlankPressed", gs.getRect("ButtomWoodBlankNormal"));
	exit->connectEvent(UIElement::UIEvents::onRelease, &GameOverScene::ExitClicked, this);
	exit->setPosition(515, 325, false);
	exit->addText("Close");
	this->m_entities.push_back(exit);

	//-Cursour- In MainMenu will always be last
	gs.LoadTexture("cursor", "Data/UI/cursorGauntlet_bronze.png");
	CursorEntity* ce = new CursorEntity(nullptr, "cursor", gs.getRect("cursor"));
	this->m_entities.push_back(ce);

	//Make scene not deleting teirselves
	this->m_deleting = false;

	//Set the Sceen manager referece
	this->m_sceneM = &sm;

	//Set the loading state of the scene
	this->m_isLoaded = true;
}

void GameOverScene::unloadSceneData()
{
	//Check if scene is loaded before unload
	if (!m_isLoaded)
	{
		return;
	}

	GRAPHICS::GraphicsSystem& gs = this->m_sceneM->getSimulation().getGraphicsSystem();
	//Clear memory!
	for (std::vector<Entity*>::iterator it = this->m_entities.begin(); it != this->m_entities.end();)
	{
		if (*it != nullptr)
		{
			delete *it;
			it = this->m_entities.erase(it);
		}
		else
		{
			it++;
		}
	}

	//set scene to unloaded
	this->m_isLoaded = false;
}

void GameOverScene::setParent(BaseScene * parent)
{
	this->m_parent = parent;
}

void GameOverScene::MainMenuClicked()
{
	this->m_deleting = true;
}

void GameOverScene::ExitClicked()
{
	this->m_sceneM->getSimulation().stop();
}
