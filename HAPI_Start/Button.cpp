#include "Button.hpp"
#include "HAPI_lib.h"
#include "GraphicsSystem.hpp"
#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace HAPISPACE;
Button::Button(std::string normalId, std::string hoverId, std::string pressedId, Rectangle colissionVolume) : UIElement(normalId, colissionVolume),
																											  m_normalIdentifier(normalId), m_hoverIdentifier(hoverId),
																											  m_pressedIdentifier(pressedId)
{
	this->connectEvent(UIEvents::onEnter, &Button::onEnter, this);
	this->connectEvent(UIEvents::onExit, &Button::onExit, this);
	this->connectEvent(UIEvents::onPress, &Button::onPress, this);
	this->connectEvent(UIEvents::onRelease, &Button::onRelease, this);
}


Button::~Button()
{
}

void Button::onEnter()
{
	this->m_identifier = this->m_hoverIdentifier;
}

void Button::onExit()
{
	this->m_identifier = this->m_normalIdentifier;
}

void Button::onPress()
{
	this->m_identifier = this->m_pressedIdentifier;
}

void Button::onRelease()
{
	this->m_identifier = this->m_hoverIdentifier;
}

