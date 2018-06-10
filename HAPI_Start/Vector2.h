#pragma once
#include <cmath>
#include <iostream>

class Vector2
{
public:
	// In small classes like this it is OK to make data public
	float x{ 0 };
	float y{ 0 };

	// Constructors
	Vector2() = default;
	Vector2(float newX, float newY) : x(newX), y(newY) {}

	// + operator. Returns the result of adding the rhs to this
	Vector2 operator+(const Vector2 &rhs) const
	{
		return Vector2(x + rhs.x, y + rhs.y);
	}

	// - operator. Returns the result of subtracting the rhs from this
	Vector2 operator - (const Vector2 &rhs) const
	{
		return Vector2(this->x - rhs.x, this->y - rhs.y);
	}

	// * operator. Returns the result of multiplying the rhs scaler to this
	Vector2 operator * (float rhs) const
	{
		return Vector2(this->x * rhs, this->y * rhs);
	}

	// *= operator. Alters this to be the result of multiplying by the rhs scaler. 
	// Returns a reference to this for chaining purposes
	Vector2& operator *= (float rhs)
	{
		this->x = this->x * rhs;
		this->y = this->y * rhs;
		return *this;
	}

	//Substract the x and y from the currecnt vector by the right hand side
	Vector2& operator-= (const Vector2& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}
	//Adds the x and y from the currecnt vector by the right hand side
	Vector2& operator+=(const Vector2& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	//Outputs the vector information
	std::ostream& operator<<(std::ostream& os)
	{
		os << "X: " << this->x << " Y:" << this->y;
		return os;
	}

	//Comparator operator
	bool operator!=(const Vector2& other)
	{
		return x != other.x || y != other.y;
	}

	// Returns the length (magnitude) of this vector
	// add a SquaredLength fn that returns the squared length for use in comparisons etc.
	float EclisiveLength() const
	{
		return sqrt(x*x + y*y);
	}

	float EclisiveLengthSquared() const
	{
		return (x*x + y * y);
	}

	float manhattanDistance() const
	{
		return std::abs(x) + std::abs(y);
	}

	// Normalises this vector so its length is 1
	// The resulting vector is known as a unit vector
	// Useful when you wish to just use the vector as a direction
	void NormaliseInPlace()
	{
		float len = EclisiveLength();

		if (len == 0)
			return;

		x /= len;
		y /= len;
	}

	/*
	The dot product can be used to calculate the angle between 2 vectors.
	If both vectors are already unit vectors then the dot product returns the
	cosine of the angle between them. If they are not unit vectors then you must divide
	the result by the product of the vector lengths to get the cosine of the angle.
	Another use of the dot product is to find the distance of a point to a plane
	*/
	float Dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	/*
	The cross product gives a value which is 2 times the area of the triangle.
	Can also be used to find which rotation direction a vector is, relative to another (+-)
	*/
	float Cross(const Vector2 &other) const
	{
		return x * other.y - y * other.x;
	}

};