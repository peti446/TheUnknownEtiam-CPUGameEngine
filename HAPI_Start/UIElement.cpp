#include "UIElement.hpp"
#include "GraphicsSystem.hpp"
#include "SoundManager.h"

#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

UIElement::UIElement(std::string Identifier, Rectangle colissionVolume, bool realtiveToScreen) : Entity(Identifier, colissionVolume),
																		  m_lastUpdatePressed(false),
																		  m_lastUpdateInside(false),
																		  m_collisionExecuted(false),
																		  m_relativeToScreen(realtiveToScreen)
{
	this->m_side = Entity::eSide::UI;
}


UIElement::~UIElement()
{
}

void UIElement::addText(const std::string& text, const HAPI_TColour colour, const int textSize, const unsigned int style, const Text::eUITextAlignmentHoriz horizoAligment, const Text::eUITextAlignmentVert vertAligment, const int horizoOffset, const int vertOffset)
{
	this->m_texts.push_back(Text(text, this->m_colisionVolume, textSize, colour, style, horizoAligment, vertAligment, horizoOffset, vertOffset));
}

void UIElement::setRelativeToScreen(const bool relative)
{
	this->m_relativeToScreen = relative;
}

void UIElement::setEntered(const bool hovering)
{
	if (hovering)
	{
		//Entering the button, check if onEnter needs to be executed
		if (!this->m_lastUpdateInside)
		{
			this->m_lastUpdateInside = true;
			//Execute on enter
			for (auto f : this->m_onEnterFuncs)
			{
				f();
			}
		}
	}
	else
	{
		//Not hovering so reset, however still check if onExit needs to be executed
		if (this->m_lastUpdateInside)
		{
			this->m_lastUpdateInside = false;
			this->m_lastUpdatePressed = false;
			//Execute on exit as we were inside the UI elements before
			for (auto f : this->m_onExitFuncs)
			{
				f();
			}
		}
	}
	this->m_collisionExecuted = true;
}

void UIElement::setPressed(const bool pressed)
{
	if (this->m_lastUpdateInside)
	{
		if (!this->m_lastUpdatePressed && pressed)
		{
			//Execute on press
			for (auto f : this->m_onPressFuncs)
			{
				f();
			}
		}
		else if(this->m_lastUpdatePressed && !pressed)
		{
			//Execute on release
			for (auto f : this->m_onReleaseFuncs)
			{
				f();
			}
		}
		this->m_lastUpdatePressed = pressed;
	}
}

void UIElement::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;

	if (!this->m_collisionExecuted && this->m_lastUpdateInside)
	{
		this->m_lastUpdateInside = false;
		this->m_lastUpdatePressed = false;
		//On exit
		for (auto f : this->m_onExitFuncs)
		{
			f();
		}
	}
	this->m_collisionExecuted = false;

	if (this->m_relativeToScreen)
	{
		if (cameraRect != this->m_cameraRect)
		{
			Vector2 cameraTransform = Vector2((float)cameraRect.getLeft() - this->m_cameraRect.getLeft(), (float)cameraRect.getTop() - this->m_cameraRect.getTop());
			Vector2 newUIPosition = this->m_position + cameraTransform;
			this->setPosition(newUIPosition, false);
			this->m_cameraRect = cameraRect;
		}
	}
}

void UIElement::draw(const GRAPHICS::GraphicsSystem & gs, float interV)
{
	Entity::draw(gs, interV);
	if (this->m_active)
	{
		this->drawTexts();
	}
}

void UIElement::drawTexts() const
{
	//To make a text reltive to the screen we simply remove the camera position from the current UI element position.
	//To make a text not relative to screen but realite to world space, we need to check if the text should be visible, and because the text render function is
	// always relative to screen, we need to make it relative if we should see it so we can see it.
	Vector2 relativePos = this->m_position;
	if (this->m_relativeToScreen)
	{
		//Substract camera position
		relativePos.x -= this->m_cameraRect.getLeft();
		relativePos.y -= this->m_cameraRect.getTop();
	}
	else
	{
		if (relativePos.x > this->m_cameraRect.getLeft() && relativePos.x < this->m_cameraRect.getWidth() + this->m_cameraRect.getLeft()
			&&
			relativePos.y > this->m_cameraRect.getTop() && relativePos.y < this->m_cameraRect.getHeight() + this->m_cameraRect.getTop())
		{
			relativePos.x -= this->m_cameraRect.getLeft();
			relativePos.y -= this->m_cameraRect.getTop();
		}
	}

	//Render all texts in the vector
	for (const Text& t : this->m_texts)
	{
		HAPI.RenderText((int)(t.getTextRelativePosition().x + relativePos.x), (int)(t.getTextRelativePosition().y + relativePos.y), t.getColor(), t.getText(), t.getTextSize(), t.getTextStyle());
	}
}
