#include "Rectangle.hpp"
/*
* Constructor and destructor
*/
Rectangle::Rectangle()
{
	this->m_left = 0;
	this->m_right = 0;
	this->m_top = 0;
	this->m_bottom = 0;
}
Rectangle::Rectangle(int left, int right, int top, int bottom)
{
	//Set the rectangle values to the given values
	this->m_left = left;
	this->m_right = right;
	this->m_top = top;
	this->m_bottom = bottom;
}

Rectangle::~Rectangle()
{
}


/*
* ---------
*    Getters
* ---------
*/

const int Rectangle::getWidth() const
{
	return  this->m_right - this->m_left;
}

const int Rectangle::getHeight() const
{
	return this->m_bottom - this->m_top;
}

const int Rectangle::getLeft() const
{
	return this->m_left;
}

const int Rectangle::getRight() const
{
	return this->m_right;
}

const int Rectangle::getTop() const
{
	return this->m_top;
}

const int Rectangle::getBottom() const
{
	return this->m_bottom;
}

//-----

//Clip the current rectangle to the other rectangle (Sets the current points so they are inside the other rectangle)
void Rectangle::ClipTo(const Rectangle& other)
{
	this->m_left = std::max(this->m_left, other.m_left);
	this->m_right = std::min(this->m_right, other.m_right);
	this->m_top = std::max(this->m_top, other.m_top);
	this->m_bottom = std::min(this->m_bottom, other.m_bottom);
}

//Checks if the actual rectangle contains compleatly other rectangle
bool Rectangle::CompleatlyContains(const Rectangle & other) const
{
	if (this->m_left >= other.m_left && this->m_right <= other.m_right
		&&
		this->m_top >= other.m_top && this->m_bottom <= other.m_top)
	{
		return true;
	}

	return false;
}

//Checks if the actual rectangle is compleatly ourside other rectangle
bool Rectangle::CompleatlyOutise(const Rectangle & other) const
{
	if(this->m_left > other.m_right)
		return true;

	if(this->m_top > other.m_bottom) 
		return true;

	if(this->m_right < other.m_left)
		return true;

	if(this->m_bottom < other.m_top)
		return true;

	return false;
}

//Translate the current rectanglo to the specific position
void Rectangle::translate(int dx, int dy)
{
	this->m_left += dx;
	this->m_right += dx;
	this->m_top += dy;
	this->m_bottom += dy;
}

void Rectangle::reduceRectangleTo(const float widthPorcengare, const float heightPorcentage)
{
	int yTrans = this->getHeight() - (int)((float)this->getHeight() * widthPorcengare);
	int xTrans = this->getWidth() - (int)((float)this->getWidth() * heightPorcentage);

	this->m_bottom -= yTrans;
	this->m_top += yTrans;
	this->m_right -= xTrans;
	this->m_left += xTrans;
}

bool Rectangle::operator==(const Rectangle& other) const
{
	return (this->m_left == other.m_left) && (this->m_right == other.m_right) && (this->m_top == other.m_top) && (this->m_bottom == other.m_bottom);
}

bool Rectangle::operator!=(const Rectangle & other) const
{
	return (this->m_left != other.m_left) || (this->m_right != other.m_right) || (this->m_top != other.m_top) || (this->m_bottom != other.m_bottom);
}
