#pragma once
#include "UIElement.hpp"

class ProgresBar;

class EnemyInfoPanel : public UIElement
{
public:
	EnemyInfoPanel(GRAPHICS::GraphicsSystem& gs, const Vector2& pos);
	virtual ~EnemyInfoPanel();

	void setHp(const int cv, const int maxValue);
	void setMP(const int cv, const int maxValue);
	void setCurrentLevel(int cl);

	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override final;
	virtual void draw(const GRAPHICS::GraphicsSystem& gs, float interV) override final;

	virtual void setActive(bool active) override final;
	virtual void setPosition(const Vector2& pos, const bool inter) override final;

private:
	//UI elements
	ProgresBar* m_hpBar;
	ProgresBar* m_manaBar;
};

