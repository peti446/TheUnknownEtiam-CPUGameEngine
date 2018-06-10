#pragma once

#include <algorithm>

class Rectangle
{
public:
	/*
	* Constructor and destructor
	*/
	Rectangle();
	Rectangle(int left, int right, int top, int bottom);
	~Rectangle();

	/*
	*	Geters
	*/
	const int getWidth() const;
	const int getHeight() const;
	const int getLeft() const;
	const int getRight() const;
	const int getTop() const;
	const int getBottom() const;
	// -----

	/**
	* Clips the current rectangle to other retangle
	* Paraemetrs:
	* const Rectangle& other - The other rectangle where this rectangle get cliped to.
	*
	* Return:
	* void
	**/
	void ClipTo(const Rectangle& other);
	/**
	* Checks if the other rectangle is compleatly inside the current rectangle
	* Paraemetrs:
	* const Rectangle& other - The other rectangle.
	*
	* Return:
	* bool - If the other rectangle is compleaty inside this one
	**/
	bool CompleatlyContains(const Rectangle& other) const;
	/**
	* Checks if this rectangle is compleatly outside the other rectangle
	* Paraemetrs:
	* const Rectangle& other - The other rectangle.
	*
	* Return:
	* bool - If the current rectangle is compleaty outside the other one
	**/
	bool CompleatlyOutise(const Rectangle& other) const;
	/**
	* Translates the current rectangle to the specific location
	* Paraemetrs:
	* int dx - X Position to translate the rectangle.
	* int dy - y Position to translate the rectangle.
	*
	* Return:
	* void
	**/
	void translate(int dx, int dy);

	/**
	* Reduces the rectangle width and height to a specific porcentage, and translates the recangle so its center is still in the place where it should be with full with and height
	* Paraemetrs:
	* float widthPorcentage - The target porcentage
	* float heightPorcentage -  The target porcentage
	*
	* Return:
	* void
	**/
	void reduceRectangleTo(const float widthPorcengare, const float heightPorcentage);

	/*
	* Operator == to check if two rectangles are the same
	*/
	bool operator==(const Rectangle& other) const;

	/*
	* Operator != to check if two rectangles are different
	*/
	bool operator!=(const Rectangle& other) const;
private:
	//The four values 
	int m_left;
	int m_right;
	int m_top;
	int m_bottom;
};

