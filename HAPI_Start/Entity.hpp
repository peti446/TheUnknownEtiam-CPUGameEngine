#pragma once

#include <string>
#include <HAPI_lib.h>
#include "Vector2.h"
#include "Rectangle.hpp"

namespace GRAPHICS
{
	class GraphicsSystem;
};

namespace SOUND
{
	class SoundManager;
}

using namespace HAPISPACE;

class Entity
{
public:
	enum class eSide
	{
		Player, Enemy, Spell, NoColision, UI, Cursor, DEFAULT
	};

	Entity(std::string identifier, Rectangle colissionVolume, int frameNum = 0);
	virtual ~Entity();

	/*
	* Abstract functions that each child need ot implement by themselves
	*/
	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) = 0;
	//Functions that can be overwrriten but got default implementation
	virtual void draw(const GRAPHICS::GraphicsSystem& gs, float interV);
	virtual void collision(Entity& other);
	virtual void setActive(const bool active);
	virtual void setPosition(const Vector2& pos, const bool Inter = true);
	virtual bool isColliding(const Entity& other) const;
	virtual float getDamage() const;
	virtual void takeDamage(const float amount);

	bool canColideWith(const eSide s1, const eSide s2) const;
	eSide getSide() const;
	void setPosition(const int x, const int y, const bool Inter = true);
	bool isActive() const;


	/*
	*  Functions common to all entities
	*/
	const Vector2 getPosition() const;
	const Vector2 getPreviousPosition() const;
	const std::string& getIdentifier() const;
	const Rectangle& getColisionVolume() const;

protected:
	//General variables needed to draw, update and collision
	Vector2 m_position;
	std::string m_identifier;
	Rectangle m_colisionVolume;
	//Animation variables
	int m_framNumber;
	int m_endFrameNum;
	int m_startFrameNum;
	DWORD m_lastTimeAnimUpdate;
	DWORD m_animTime;

	//Specific information of the entity
	eSide m_side;
	bool m_active;
	float m_speed;
	//Variables to control the entity
	//Health: -66666 infinite helth, 0 dead, > 0 not dead
	float m_health;
	float m_damage;
private:
	Vector2 m_previousPostion;
	bool m_interpolate;
};

