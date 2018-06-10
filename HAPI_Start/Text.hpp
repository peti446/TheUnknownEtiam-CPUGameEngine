#pragma once

#include <string>
#include <HAPI_lib.h>
#include "Rectangle.hpp"
#include "Vector2.h"

using namespace HAPISPACE;

class Text
{
public:
	enum class eUITextAlignmentVert
	{
		Top, Center, Bottom
	};
	enum class eUITextAlignmentHoriz
	{
		Left, Center, Right
	};
public:
	Text(const std::string& text,
		 const Rectangle relativeRect,
		 const int textSize = 18,
		 const HAPI_TColour colour = HAPI_TColour::BLACK,
		 const unsigned int style = 0,
		 const eUITextAlignmentHoriz horizoAligment = eUITextAlignmentHoriz::Center,
		 const eUITextAlignmentVert vertAligment = eUITextAlignmentVert::Center,
		 const int horizoOffset = 0,
		 const int vertOffset = 0);
	~Text();

	/*
	* Setters
	*/
	void setText(const std::string& text);
	void setRelativeRect(const Rectangle relativeRect);
	void setTextSize(const int textSize);
	void setColor(const HAPI_TColour colour);
	void setStyle(const unsigned int style);
	void setHorizontalAligment(const eUITextAlignmentHoriz horizontalAligment);
	void setVerticalAligment(const eUITextAlignmentVert verticalAligment);
	void setHorizontalOffset(const int horizontalOffset);
	void setVerticalOffset(const int verticalOffset);

	/*
	* Getters for some elemets of the text
	*/
	const std::string& getText() const;
	const int getTextSize() const;
	const HAPI_TColour getColor() const;
	const Vector2 getTextRelativePosition() const;
	const unsigned int getTextStyle() const;
private:
	//Private functions for reusability
	void calculateTextRelativePosition();
	void calculateHorizontalRelativePosition();
	void calculateVerticalRelativePosition();
	void reCalculateTextRect();


	//Variables for each text will be able to update them one by one later one
	std::string m_text;
	int m_textSize;
	HAPI_TColour m_color;
	unsigned int m_style;
	Rectangle m_relativeRect;
	Rectangle m_textRect;
	eUITextAlignmentVert m_verticalAligment;
	eUITextAlignmentHoriz m_horizontalAligment;
	int m_xOffset;
	int m_yOffset;
	int m_posX;
	int m_posY;

};

