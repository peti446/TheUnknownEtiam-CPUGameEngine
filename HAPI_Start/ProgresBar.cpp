#include "ProgresBar.hpp"
#include "GraphicsSystem.hpp"
#include <iomanip>


ProgresBar::ProgresBar(std::string leftCorner, std::string middle, std::string rightCorner, const int width, const int height, GRAPHICS::GraphicsSystem& gs) : UIElement(leftCorner, Rectangle(0,width,0,height)),
																																				   m_middleIdentifier(middle),
																																				   m_rightIdentifier(rightCorner),
																																				   m_fillWith(width)
{
	//Rezise the main elemeents to fit the height and width
	gs.resizeTexture(leftCorner, gs.getRect(leftCorner).getWidth(), height);
	gs.resizeTexture(middle, gs.getRect(middle).getWidth(), height);
	gs.resizeTexture(rightCorner, gs.getRect(rightCorner).getWidth(), height);

	//Get the rectangles
	this->m_leftCornerRect = gs.getRect(leftCorner);
	this->m_middleRect = gs.getRect(middle);
	this->m_rightRect = gs.getRect(rightCorner);

	//Create the backgrounds
	gs.LoadTexture(leftCorner + "background", "Data/UI/barBack_horizontalLeft.png");
	gs.resizeTexture(leftCorner + "background", gs.getRect(leftCorner).getWidth() + 2, gs.getRect(leftCorner).getHeight() + 2);

	gs.LoadTexture(middle + "background", "Data/UI/barBack_horizontalMid.png");
	gs.resizeTexture(middle + "background", gs.getRect(middle).getWidth(), gs.getRect(middle).getHeight() + 2);

	gs.LoadTexture(rightCorner + "background", "Data/UI/barBack_horizontalRight.png");
	gs.resizeTexture(rightCorner + "background", gs.getRect(rightCorner).getWidth() + 2, gs.getRect(rightCorner).getHeight() + 2);

	this->m_texts.push_front(Text("", this->m_colisionVolume, 14));
}


ProgresBar::~ProgresBar()
{
}

void ProgresBar::setValue(const float porcent, const bool showPorcentText)
{
	float p = std::max(0.f, std::min(1.0f, porcent));
	this->m_currentValue = p;
	this->m_maxValue = 1.f;
	this->m_fillWith = (int)(this->m_colisionVolume.getWidth() * p);
	if (showPorcentText)
	{
		this->m_texts.front().setText(std::to_string((int)(p * 100.f)) + "% / 100%");
	}
	else
	{
		this->m_texts.front().setText("");
	}
}

void ProgresBar::setValue(const int currentValue, const int maxValue, const bool showValues)
{
	float p = std::max(0.f, std::min(1.0f, (float)currentValue / (float)maxValue));
	this->m_currentValue = (float)currentValue;
	this->m_maxValue = (float)maxValue;
	this->m_fillWith = (int)(this->m_colisionVolume.getWidth() * p);
	if (showValues)
	{
		std::string curentValueString = std::to_string(currentValue);
		std::string maxValueString = std::to_string(maxValue);
		std::ostringstream out;

		//Current Value change int format
		if (currentValue > 100000)
		{
			std::ostringstream out;
			out << std::fixed << std::setprecision(2) << (float)currentValue * 0.000001;
			curentValueString = out.str() + " m";
			out.str(std::string());
		}
		else if(currentValue > 10000)
		{
			out << std::fixed << std::setprecision(2) << (float)currentValue * 0.001;
			curentValueString = out.str() + " k";
			out.str(std::string());
		}

		//Max value change int format
		if (maxValue > 1000000)
		{
			out << std::fixed << std::setprecision(2) << (float)maxValue * 0.000001;
			maxValueString = out.str() + " m";
		}
		else if (maxValue > 10000)
		{
			out << std::fixed << std::setprecision(2) << (float)maxValue * 0.001;
			maxValueString = out.str() + " k";
		}

		this->m_texts.front().setText(curentValueString + " / " + maxValueString);
	}
	else
	{
		this->m_texts.front().setText("");
	}
}

float ProgresBar::getCurrentValue() const
{
	return this->m_currentValue;
}

float ProgresBar::getMaxValue() const
{
	return this->m_maxValue;
}

void ProgresBar::draw(const GRAPHICS::GraphicsSystem & gs, float interV)
{
	//Draw all elements beside the first one
	if (this->m_active)
	{
		//Get the position of the element
		Vector2 currentPos = this->m_position;

		//Draw Backgrounds
		{
			gs.Draw(this->m_identifier + "background", (int)(currentPos.x - 1), (int)(currentPos.y - 1));
			const int endPartStartX = (int)(currentPos.x + this->m_colisionVolume.getWidth() - this->m_rightRect.getWidth()) - 1;
			gs.Draw(this->m_rightIdentifier + "background", endPartStartX, (int)(currentPos.y - 1));
			//Draw middle backgrounds
			for (float x = currentPos.x + this->m_leftCornerRect.getWidth() + 1; x < endPartStartX; x += this->m_middleRect.getWidth())
			{
				//Calculate the porcentage of the center pice with to draw if we overshoot the central limit
				float porcentWidth = 1.0f;
				if ((x + this->m_middleRect.getWidth()) >= endPartStartX)
				{
					float amountToMuch = (x + this->m_middleRect.getWidth()) - endPartStartX+1;
					porcentWidth = 1 - (amountToMuch / this->m_middleRect.getWidth());
				}
				gs.Draw(this->m_middleIdentifier + "background", (int)x, (int)currentPos.y - 1, 0, porcentWidth);
			}
		}

		//Draw the actual bar

		//Get the left and right fill porcentage (in case the porcentage is lower then the width of both together), so that we can get a smooth good looking bar at lower %
		float widthFillPorcent = 1.0f;
		if (this->m_fillWith < this->m_leftCornerRect.getWidth() + this->m_rightRect.getWidth())
		{
			widthFillPorcent = ((float)(this->m_fillWith) / (float)(this->m_leftCornerRect.getWidth() + this->m_rightRect.getWidth()));
		}
		
		//Draw left corner
		gs.Draw(this->m_identifier, currentPos, 0, widthFillPorcent);
		//Draw Right corner
		currentPos.x += (float)this->m_fillWith - (float)(this->m_rightRect.getWidth());
		gs.Draw(this->m_rightIdentifier, currentPos, 0, widthFillPorcent < 1.0f ? -widthFillPorcent : widthFillPorcent);

		//Draw all the center pices starting at the end of the left corner
		if (widthFillPorcent == 1.0f)
		{
			float startCenterfillX = this->m_position.x + (this->m_leftCornerRect.getWidth() * widthFillPorcent);
			for (float x = startCenterfillX; x < currentPos.x; x += this->m_middleRect.getWidth())
			{
				//Calculate the porcentage of the center pice with to draw if we overshoot the central limit
				float porcentWidth = 1.0f;
				if ((x + this->m_middleRect.getWidth()) > currentPos.x)
				{
					float amountToMuch = (x + this->m_middleRect.getWidth()) - currentPos.x;
					porcentWidth = 1 - (amountToMuch / this->m_middleRect.getWidth());
				}
				gs.Draw(this->m_middleIdentifier, (int)x, (int)currentPos.y, 0, porcentWidth);
			}
		}
		//Draw the text
		UIElement::drawTexts();
	}
}
