#pragma once
#include "BaseScene.hpp"

class MainMenu : public BaseScene
{
public:
	MainMenu();
	virtual ~MainMenu();

	virtual void updateScene() override final;
	virtual void drawScene(float interV) override final;
	virtual void loadScene(SceneManager& sm) override final;
	virtual void unloadSceneData() override final;

	void PlayOnclick();
	void ExitOnClick();
};

