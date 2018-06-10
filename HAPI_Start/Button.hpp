#pragma once
#include "UIElement.hpp"
class Button : public UIElement
{
public:
	Button(std::string normalId, std::string hoverId, std::string pressedId, Rectangle colissionVolume);
	virtual ~Button();

	void onEnter();
	void onExit();
	void onPress();
	void onRelease();

private:
	std::string m_normalIdentifier;
	std::string m_hoverIdentifier;
	std::string m_pressedIdentifier;
	std::string m_text;

};

