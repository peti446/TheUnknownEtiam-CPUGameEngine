#include "Entity.hpp"
#include "GraphicsSystem.hpp"

Entity::Entity(std::string identifier, Rectangle colissionVolume, int frameNum) : m_position(0, 0), m_previousPostion(0,0),
																	m_identifier(identifier), m_colisionVolume(colissionVolume),
																	m_framNumber(frameNum), m_side(Entity::eSide::DEFAULT),
																	m_active(true), m_health(-66666.f), m_speed(0.0f), m_damage(0.f),
																	m_interpolate(false), m_endFrameNum(0), m_startFrameNum(0),
																	m_lastTimeAnimUpdate(HAPI.GetTime()), m_animTime(500)
{
}

Entity::~Entity()
{
}

void Entity::draw(const GRAPHICS::GraphicsSystem& gs, float interV)
{
	if (!this->m_active || this->m_framNumber < 0)
		return;
	
	
	Vector2 interPos = this->m_position;
	if (this->m_interpolate)
	{
		if (interV >= 1.0f)
			this->m_interpolate = false;
		interPos = this->m_previousPostion + ((this->m_position - this->m_previousPostion) * interV);
	}

	gs.Draw(this->m_identifier, interPos, this->m_framNumber);

	if (this->m_startFrameNum != this->m_endFrameNum)
	{
		//Clamp frame number to start and end frame
		this->m_framNumber = std::max(this->m_startFrameNum, std::min(this->m_framNumber, this->m_endFrameNum));
		if (HAPI.GetTime() - this->m_lastTimeAnimUpdate >= this->m_animTime)
		{
			this->m_framNumber++;
			if (this->m_framNumber > this->m_endFrameNum)
			{
				this->m_framNumber = this->m_startFrameNum;
			}
			this->m_lastTimeAnimUpdate = HAPI.GetTime();
		}
	}
}

void Entity::collision(Entity & other)
{
	Rectangle thisRect(this->m_colisionVolume);
	thisRect.reduceRectangleTo(0.9f, 0.9f);
	Rectangle otherRect(other.m_colisionVolume);
	otherRect.reduceRectangleTo(0.9f, 0.9f);

	Vector2 thisspeed(this->m_previousPostion - this->m_position);
	thisspeed.NormaliseInPlace();
	thisspeed *= this->m_speed;
	Vector2 otherSpeed(other.m_previousPostion - other.m_position);
	otherSpeed.NormaliseInPlace();
	otherSpeed *= other.m_speed;

	Vector2 thisPosition = this->m_position;
	Vector2 otherPosition = other.m_position;

	if ((thisspeed.x != 0 || thisspeed.y != 0) || (otherSpeed.x != 0 || otherSpeed.y != 0))
	{
		do
		{
			thisPosition += thisspeed;
			otherPosition += otherSpeed;
			thisRect = Rectangle((int)thisPosition.x, (int)thisPosition.x + thisRect.getWidth(), (int)thisPosition.y, (int)thisPosition.y + thisRect.getHeight());
			otherRect = Rectangle((int)otherPosition.x, (int)otherPosition.x + otherRect.getWidth(), (int)otherPosition.y, (int)otherPosition.y + otherRect.getHeight());
		} while (!thisRect.CompleatlyOutise(otherRect));

		this->setPosition(thisPosition, false);
		other.setPosition(otherPosition, false);
		this->m_previousPostion = this->m_position;
		other.m_previousPostion = other.m_position;
	}
}

bool Entity::isColliding(const Entity & other) const
{
	if (!this->m_active)
		return false;

	if (!this->canColideWith(this->m_side, other.m_side))
		return false;

	Rectangle thisRect = this->m_colisionVolume;
	Rectangle otherRect = other.m_colisionVolume;
	if(this->m_side != eSide::Cursor){
		thisRect.reduceRectangleTo(0.9f, 0.9f);
		otherRect.reduceRectangleTo(0.9f, 0.9f);
	}
	else
	{
		thisRect = Rectangle(thisRect.getLeft(), thisRect.getLeft() + 6, thisRect.getTop(), thisRect.getTop() + 6);
	}

	return !thisRect.CompleatlyOutise(otherRect);
}

float Entity::getDamage() const
{
	return (float)this->m_damage;
}

bool Entity::canColideWith(const eSide s1, const eSide s2) const
{
	if ((s1 == eSide::Player && s2 == eSide::DEFAULT) || (s2 == eSide::Player && s1 == eSide::DEFAULT))
		return true;
	if ((s1 == eSide::Enemy && s2 == eSide::DEFAULT) || (s2 == eSide::Enemy && s1 == eSide::DEFAULT))
		return true;
	if ((s1 == eSide::Cursor && s2 == eSide::UI) || (s1 == eSide::Cursor && s2 == eSide::Enemy))
		return true;
	if ((s1 == eSide::Player && s2 == eSide::Enemy) || (s1 == eSide::Enemy && s2 == eSide::Player))
		return true;
	if ((s1 == eSide::Enemy && s2 == eSide::Enemy))
		return true;
	if ((s1 == eSide::Spell && s2 == eSide::Enemy))
		return true;
	if ((s1 == eSide::Spell && s2 == eSide::Player))
		return true;
	return false;
}

Entity::eSide Entity::getSide() const
{
	return this->m_side;
}

void Entity::setPosition(const int x, const int y, const bool Inte)
{
	this->setPosition(Vector2((float)x, (float)y), Inte);
}

void Entity::setPosition(const Vector2& pos, const bool Inter)
{
	this->m_colisionVolume.translate((int)pos.x - this->m_colisionVolume.getLeft(), (int)pos.y - this->m_colisionVolume.getTop());
	this->m_previousPostion = this->m_position;
	this->m_position = pos;
	this->m_interpolate = false;
}

bool Entity::isActive() const
{
	return this->m_active;
}

void Entity::takeDamage(const float amount)
{
	if (this->m_health != -66666.f)
	{
		this->m_health = std::max(0.f, this->m_health - amount);
		if (this->m_health == 0.f)
		{
			this->m_active = false;
		}
	}
}

void Entity::setActive(const bool active)
{
	this->m_active = active;
}

const Vector2 Entity::getPosition() const
{
	return this->m_position;
}

const Vector2 Entity::getPreviousPosition() const
{
	return this->m_previousPostion;
}

const std::string & Entity::getIdentifier() const
{
	return this->m_identifier;
}

const Rectangle & Entity::getColisionVolume() const
{
	return this->m_colisionVolume;
}
