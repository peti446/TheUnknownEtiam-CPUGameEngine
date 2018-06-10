#pragma once

#include <functional>
#include <HAPI_lib.h>
#include <deque>
#include "Entity.hpp"
#include "Text.hpp"

class UIElement : public Entity
{
public:
	enum class UIEvents
	{
		onEnter, onExit, onPress, onRelease
	};
public:
	UIElement(std::string Identifier, Rectangle colissionVolume, bool realtiveToScreen = true);
	virtual ~UIElement();

	void addText(const std::string& text,
				 const HAPI_TColour colour = HAPI_TColour::BLACK,
				 const int textSize = 18,
				 const unsigned int style = HAPI_TextStyle::eRegular,
				 const Text::eUITextAlignmentHoriz horizoAligment = Text::eUITextAlignmentHoriz::Center,
				 const Text::eUITextAlignmentVert vertAligment = Text::eUITextAlignmentVert::Center,
				 const int horizoOffset = 0,
				 const int vertOffset = 0);

	void setRelativeToScreen(const bool relative);

	//They reset every update to false
	void setEntered(const bool hovering);
	void setPressed(const bool pressed);

	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override;
	virtual void draw(const GRAPHICS::GraphicsSystem& gs, float interV) override;

	template<class F, class... Args>
	void connectEvent(UIEvents UIevent, F f, Args&&... args)
	{
		switch (UIevent)
		{
			case UIEvents::onEnter:
				this->m_onEnterFuncs.push_back(std::bind(f, std::forward<Args>(args)...));
				break;
			case UIEvents::onExit:
				this->m_onExitFuncs.push_back(std::bind(f, std::forward<Args>(args)...));
				break;
			case UIEvents::onPress:
				this->m_onPressFuncs.push_back(std::bind(f, std::forward<Args>(args)...));
				break;
			case UIEvents::onRelease:
				this->m_onReleaseFuncs.push_back(std::bind(f, std::forward<Args>(args)...));
				break;
		}
	}

protected:
	//Functions
	void drawTexts() const;


	//Variables
	std::deque<Text> m_texts;
	std::vector<std::function<void()>> m_onEnterFuncs;
	std::vector<std::function<void()>> m_onExitFuncs;
	std::vector<std::function<void()>> m_onPressFuncs;
	std::vector<std::function<void()>> m_onReleaseFuncs;
	bool m_lastUpdatePressed;
	bool m_lastUpdateInside;
	bool m_collisionExecuted;
	bool m_relativeToScreen;
	Rectangle m_cameraRect;
};

