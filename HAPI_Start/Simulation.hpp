#pragma once

#include <HAPI_lib.h>
#include "SceneManager.hpp"

namespace GRAPHICS {
	class GraphicsSystem;
};
namespace SOUND
{
	class SoundManager;
};

using namespace HAPISPACE;

namespace SIMULATION {

	class Simulation
	{
	public:
		Simulation(int width, int height);
		~Simulation();
		void start();
		void stop();
		/*
		*	Getters
		*/
		GRAPHICS::GraphicsSystem& getGraphicsSystem() const;
		SOUND::SoundManager& getSoundManager() const;
		const DWORD getTickInterval() const;
	private:
		void gameLoop();

		//Variables
		GRAPHICS::GraphicsSystem* m_graphicsSystem;
		SOUND::SoundManager* m_soundManager;
		SceneManager* m_sceneManager;
		const DWORD m_tickInterval;
	};
}
