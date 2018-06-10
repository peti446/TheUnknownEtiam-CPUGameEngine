#include "Text.hpp"


Text::Text(const std::string& text,
		   const Rectangle relativeRect,
		   const int textSize,
		   const HAPI_TColour color,
		   const unsigned int style,
		   const eUITextAlignmentHoriz horizoAligment,
		   const eUITextAlignmentVert vertAligment,
		   const int horizoOffsets,
		   const int vertOffset)
{
	//Set relative rect
	this->m_relativeRect = relativeRect;
	//Set size
	this->m_textSize = textSize;
	//setColour
	this->m_color = color;
	//Set Text style
	this->m_style = style;
	//Set te aligments
	this->m_horizontalAligment = horizoAligment;
	this->m_verticalAligment = vertAligment;
	//Set the offsets
	this->m_xOffset = horizoOffsets;
	this->m_yOffset = vertOffset;

	//Set text
	this->setText(text);
}


Text::~Text()
{
}

void Text::setText(const std::string & text)
{
	//Update text string
	this->m_text = text;
	this->reCalculateTextRect();
}

void Text::setRelativeRect(const Rectangle relativeRect)
{
	this->m_relativeRect = relativeRect;
	this->calculateTextRelativePosition();
}

void Text::setTextSize(const int textSize)
{
	this->m_textSize = textSize;
	this->reCalculateTextRect();
}

void Text::setColor(HAPI_TColour colour)
{
	this->m_color = colour;
}

void Text::setStyle(const unsigned int style)
{
	this->m_style = style;
	this->reCalculateTextRect();
}

void Text::setHorizontalAligment(const eUITextAlignmentHoriz horizontalAligment)
{
	this->m_horizontalAligment = horizontalAligment;
	this->calculateHorizontalRelativePosition();
}

void Text::setVerticalAligment(const eUITextAlignmentVert verticalAligment)
{
	this->m_verticalAligment = verticalAligment;
	this->calculateVerticalRelativePosition();
}

void Text::setHorizontalOffset(const int horizontalOffset)
{
	this->m_posX -= this->m_xOffset;
	this->m_posX += horizontalOffset;
}

void Text::setVerticalOffset(const int verticalOffset)
{
	this->m_posY -= this->m_yOffset;
	this->m_posY += this->m_yOffset;
}

const std::string& Text::getText() const
{
	return this->m_text;
}

const int Text::getTextSize() const
{
	return this->m_textSize;
}

const HAPI_TColour Text::getColor() const
{
	return this->m_color;
}

const Vector2 Text::getTextRelativePosition() const
{
	return Vector2((float)this->m_posX, (float)this->m_posY);
}

const unsigned int Text::getTextStyle() const
{
	return this->m_style;
}

void Text::calculateTextRelativePosition()
{
	this->calculateHorizontalRelativePosition();
	this->calculateVerticalRelativePosition();
}

void Text::calculateHorizontalRelativePosition()
{
	//Horizontal Aligment
	switch (this->m_horizontalAligment)
	{
	case Text::eUITextAlignmentHoriz::Left:
		this->m_posX = 0;
		break;
	case Text::eUITextAlignmentHoriz::Center:
		this->m_posX = (int)std::ceilf((this->m_relativeRect.getWidth() - this->m_textRect.getWidth()) / 2.f);
		break;
	case Text::eUITextAlignmentHoriz::Right:
		this->m_posX = this->m_relativeRect.getWidth() - this->m_textRect.getWidth();
		break;
	}

	//Add offset
	this->m_posX += this->m_xOffset;
	//Substact left from the text rectangle
	this->m_posX -= this->m_textRect.getLeft();
}

void Text::calculateVerticalRelativePosition()
{
	//Vertical aligment
	switch (this->m_verticalAligment)
	{
	case Text::eUITextAlignmentVert::Top:
		this->m_posY = 0;
		break;
	case Text::eUITextAlignmentVert::Center:
		this->m_posY = (int)std::ceilf((this->m_relativeRect.getHeight() - this->m_textRect.getHeight()) / 2.f);
		break;
	case Text::eUITextAlignmentVert::Bottom:
		this->m_posY = this->m_relativeRect.getHeight() - this->m_textRect.getHeight();
		break;
	}
	//Add offset
	this->m_posY += this->m_yOffset;
	//Substact top from the text rectangle
	this->m_posY -= this->m_textRect.getTop();
}

void Text::reCalculateTextRect()
{
	//Get the size of the text
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;
	//Getting the size
	HAPI.GetTextSize(this->m_text, this->m_textSize, this->m_style, left, right, top, bottom);
	//Create new rectangle
	this->m_textRect = Rectangle(left, right, top, bottom);

	//Update the relative position
	this->calculateTextRelativePosition();
}
