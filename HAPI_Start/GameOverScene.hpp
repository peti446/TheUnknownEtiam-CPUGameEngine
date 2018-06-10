#pragma once
#include "BaseScene.hpp"
class Text;
class GameOverScene :
	public BaseScene
{
public:
	GameOverScene();
	virtual ~GameOverScene();

	virtual void updateScene() override final;
	virtual void drawScene(float interV) override final;
	virtual void loadScene(SceneManager& sm) override final;
	virtual void unloadSceneData() override final;
	

	void setParent(BaseScene* parent);
	void MainMenuClicked();
	void ExitClicked();
private:
	BaseScene * m_parent;
	Text* m_gameOverText;
	bool m_deleting;
};

