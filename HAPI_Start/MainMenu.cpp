#include "MainMenu.hpp"
#include "SceneManager.hpp"
#include "Simulation.hpp"
#include "GraphicsSystem.hpp"
#include "SoundManager.h"
#include "GameScene.hpp"
#include "Entity.hpp"
#include "Button.hpp"
#include "CursorEntity.h"

MainMenu::MainMenu()
{
}


MainMenu::~MainMenu()
{
}

void MainMenu::updateScene()
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
}

void MainMenu::drawScene(float interV)
{
	//Draw every entity
	for (Entity* e : this->m_entities){
		e->draw(this->m_sceneM->getSimulation().getGraphicsSystem(), interV);
	}
}

void MainMenu::loadScene(SceneManager& sm)
{
	//Only load scene if not already loaded
	if (this->m_isLoaded) {
		if(sm.getSimulation().getSoundManager().getPlayingSoundStatus("MainMenuBagroundMusic") == HAPI_MediaStatus::eStopped)
			sm.getSimulation().getSoundManager().PlaySound("MainMenuBagroundMusic", "Data/Sounds/MainMenuTheme.ogg", sm.getSimulation().getSoundManager().getPlayingSoundOptions("MainMenuBagroundMusic"));
		return;
	}

	GRAPHICS::GraphicsSystem& gs = sm.getSimulation().getGraphicsSystem();
	//Background instanciation
	this->m_entities.push_back(new UIElement(gs.LoadTexture("Background", "Data/Background/MainMenuBG.png"), gs.getRect("Background")));

	//Title banner
	gs.LoadTexture("TitleBanner", "Data/UI/bannerScroll.png");
	gs.resizeTexture("TitleBanner", 375, 100);
	UIElement* banner = new UIElement("TitleBanner", gs.getRect("TitleBanner"));
	banner->setPosition(453, 100, false);
	banner->addText("The Unknown Etiam", HAPI_TColour::BLACK, 22, HAPI_TextStyle::eBold);
	this->m_entities.push_back(banner);

	//Loading of buttons into memory
	gs.LoadTexture("ButtomWoodBlankNormal", "Data/UI/panel_woodPaperWear.png");
	gs.resizeTexture("ButtomWoodBlankNormal", 250, 50);
	gs.LoadTexture("ButtonWoodBlankHover", "Data/UI/panel_woodPaperDetail.png");
	gs.resizeTexture("ButtonWoodBlankHover", 250, 50);
	gs.LoadTexture("ButtonWoodBlankPressed", "Data/UI/panel_woodDetail.png");
	gs.resizeTexture("ButtonWoodBlankPressed", 250, 50);

	//-Play button-
	Button* continueB = new Button("ButtomWoodBlankNormal", "ButtonWoodBlankHover", "ButtonWoodBlankPressed", gs.getRect("ButtomWoodBlankNormal"));
	continueB->connectEvent(UIElement::UIEvents::onRelease, &MainMenu::PlayOnclick, this);
	continueB->setPosition(515, 250, false);
	continueB->addText("Play!");
	this->m_entities.push_back(continueB);

	//-Exit button-
	Button* exit = new Button("ButtomWoodBlankNormal", "ButtonWoodBlankHover", "ButtonWoodBlankPressed", gs.getRect("ButtomWoodBlankNormal"));
	exit->connectEvent(UIElement::UIEvents::onRelease, &MainMenu::ExitOnClick, this);
	exit->setPosition(515, 325, false);
	exit->addText("Close");
	this->m_entities.push_back(exit);



	//-Cursour- In MainMenu will always be last
	gs.LoadTexture("cursor", "Data/UI/cursorGauntlet_bronze.png");
	CursorEntity* ce = new CursorEntity(nullptr, "cursor", gs.getRect("cursor"));
	this->m_entities.push_back(ce);

	//Start the MainTheme souund with some options
	sm.getSimulation().getSoundManager().LoadSound("Data/Sounds/MainMenuTheme.ogg");
	HAPI_TSoundOptions scfg = HAPI_TSoundOptions(0.4f, true);
	sm.getSimulation().getSoundManager().PlaySound("MainMenuBagroundMusic", "Data/Sounds/MainMenuTheme.ogg", scfg);

	//Set the Sceen manager referece
	this->m_sceneM = &sm;

	//Set the loading state of the scene
	this->m_isLoaded = true;
}

void MainMenu::unloadSceneData()
{
	//Check if scene is loaded before unload
	if (!m_isLoaded){
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

void MainMenu::PlayOnclick()
{
	this->m_sceneM->getSimulation().getSoundManager().stopSound("MainMenuBagroundMusic");
	GameScene* newGame = new GameScene();
	BaseScene * b = this->m_sceneM->changeScene(*(newGame), false);
	this->m_sceneM->storeScene("MainMenu", *b);
}

void MainMenu::ExitOnClick()
{
	this->m_sceneM->getSimulation().stop();
}
