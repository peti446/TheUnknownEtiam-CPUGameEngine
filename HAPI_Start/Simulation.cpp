#include "Simulation.hpp"
#include "GraphicsSystem.hpp"
#include "SoundManager.h"

namespace SIMULATION {

	Simulation::Simulation(int width, int height) : m_tickInterval(33),
												    m_graphicsSystem(new GRAPHICS::GraphicsSystem(width, height)),
													m_sceneManager(nullptr),
													m_soundManager(nullptr)
	{
	}


	Simulation::~Simulation()
	{
		//Clean up ORDER IS IMPORTANT
		if (this->m_sceneManager != nullptr)
			delete this->m_sceneManager;
		if(this->m_graphicsSystem != nullptr)
			delete this->m_graphicsSystem;
		if (this->m_soundManager != nullptr)
			delete this->m_soundManager;
	}

	void Simulation::start()
	{
		//Start the visualisation system
		if (!this->m_graphicsSystem->startSystem())
		{
			std::cerr << "Could not start graphycs system! Closing the application. Yhea you cannot play a game with no textures at all ?" << std::endl;
			return;
		}
		//Show FPS
		HAPI.SetShowFPS(true, 5,690);
		//Remove cursor
		HAPI.SetShowCursor(false);
		//Change the font
		HAPI.ChangeFontFromFile("Data/Fonts/BLKCHCRY.TTF");

		//Start sound manager
		this->m_soundManager = new SOUND::SoundManager();

		//Start the scene manager
		this->m_sceneManager = new SceneManager(*this);

		//Start the game loop
		this->gameLoop();
	}


	void Simulation::stop()
	{
		//Stop the game
		HAPI.Close();
	}

	void Simulation::gameLoop()
	{
		//Loop game
		DWORD currentTime = HAPI.GetTime();
		//double t = 0.0;
		double timeSinceLastTick = 0.0;
		while (HAPI.Update())
		{
			//Find time since last update
			DWORD loopTime = HAPI.GetTime();
			double lastUpdateTime = loopTime - currentTime;
			currentTime = loopTime;

			//Find out how many ticks needs to be done (in case render takes longer)
			timeSinceLastTick += lastUpdateTime;
			if (timeSinceLastTick >= 2*this->m_tickInterval)
				std::cerr << "Tick rate cant keep up, too low fps.. (Should be 30+)(Executing more than one update before the draw call)" << std::endl;
			//Catch up the ticks
			while (timeSinceLastTick >= this->m_tickInterval)
			{
				//Tick do update
				//Update active scene 
				this->m_sceneManager->updateCScene();

				//t += this->m_tickInterval;
				timeSinceLastTick -= this->m_tickInterval;
			}

			//Draw Active scene
			float interValue = std::max(0.0f, std::min(1.0f, (float)timeSinceLastTick / (float)this->m_tickInterval));
			this->m_sceneManager->drawCScene(interValue);
		}
	}

	GRAPHICS::GraphicsSystem& Simulation::getGraphicsSystem() const
	{
		return (*this->m_graphicsSystem);
	}
	SOUND::SoundManager & Simulation::getSoundManager() const
	{
		return (*this->m_soundManager);
	}
	const DWORD Simulation::getTickInterval() const
	{
		return this->m_tickInterval;
	}
}