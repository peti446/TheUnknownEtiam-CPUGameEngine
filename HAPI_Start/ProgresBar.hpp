#pragma once
#include "UIElement.hpp"
class ProgresBar : public UIElement
{
public:
	ProgresBar(std::string leftCorner,std::string middle, std::string rightCorner, const int width, const int height, GRAPHICS::GraphicsSystem& gs);
	virtual ~ProgresBar();

	void setValue(const float porcent, const bool showPorcentText);
	void setValue(const int currentValue, const int maxValue, const bool showValues = true);

	float getCurrentValue() const;
	float getMaxValue() const;

	virtual void draw(const GRAPHICS::GraphicsSystem& gs, float interV) override final;

private:
	int m_fillWith;
	float m_currentValue;
	float m_maxValue;
	Rectangle m_leftCornerRect;
	std::string m_middleIdentifier;
	Rectangle m_middleRect;
	std::string m_rightIdentifier;
	Rectangle m_rightRect;
};

