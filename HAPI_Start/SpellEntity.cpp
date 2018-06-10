#include "SpellEntity.hpp"
#include "SoundManager.h"
#include "Player.hpp"
#include "Enemy.hpp"
#include <random>

SpellEntity::SpellEntity() : Entity("", Rectangle(), 0), m_frameTime(0), m_lastTimeAnimUpdate(0)
{
	this->m_side = eSide::Spell;
	this->m_health = 1;
	this->m_speed = 5.f;
	this->m_active = false;
}


SpellEntity::~SpellEntity()
{
}

void SpellEntity::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;

	if (this->m_firstUpdate)
	{
		this->m_firstUpdate = false;
		HAPI_TSoundOptions so(0.5f, false);
		sm.PlaySound("Spell1" , "Data/Sounds/FireBall.ogg", so);
	}

	switch (this->m_type)
	{
	case eSpellType::Follow:
	{
		Vector2 direction = this->m_target->getPosition() - this->getPosition();
		direction.NormaliseInPlace();
		Vector2 newPos = this->getPosition() + (direction * this->m_speed);
		this->setPosition(newPos);
	}
		break;
	case eSpellType::SkillShot:
	{
		Vector2 newPos = this->getPosition() + (this->m_direction * this->m_speed);
		float d = (this->m_startPosition - newPos).manhattanDistance();
		//Distance not be greatr then 240
		if (d < 240.f)
		{
			this->setPosition(newPos);
		}
		else
		{
			this->m_active = false;
		}
	}
		break;
	}
}

bool SpellEntity::isColliding(const Entity & other) const
{
	if (other.getSide() == this->m_owner->getSide())
		return false;
	if (&other == this->m_owner)
		return false;
	if (this->m_type == eSpellType::Follow)
		if (&other != this->m_target)
			return false;

	return Entity::isColliding(other);
}

void SpellEntity::collision(Entity & other)
{
	other.takeDamage(this->m_damage);
	this->m_active = false;
	if (this->m_owner->getSide() == eSide::Player)
	{
		if (!other.isActive())
		{
			Player& p = (Player&)*this->m_owner;
			Enemy& e = (Enemy&)other;
			p.addXP(e.getXPReward());
		}
	}
}

void SpellEntity::Instanciate(const float damage, const Entity* const owner, const  Entity* const target, const eSpellType type, const Vector2 dir, const std::string identifier, const Rectangle colissionVolume, const DWORD frameTimeMS, const int frameNumStart, const int frameNumEnd)
{
	//Set variables for the update
	this->m_owner = owner;
	this->m_target = target;
	this->m_type = type;
	this->m_direction = dir;
	this->m_identifier = identifier;
	this->m_colisionVolume = colissionVolume;
	this->m_frameTime = frameTimeMS;
	this->m_startFrameNum = frameNumStart;
	this->m_endFrameNum = frameNumEnd;
	this->m_active = true;
	this->setPosition(owner->getPosition());
	this->m_startPosition = owner->getPosition();
	this->m_health = 1.f;
	this->m_firstUpdate = true;
	this->m_damage = damage;

	//Check if type and target mach up (only follow needs target the rest of spells can be null as it will not be used)
	if (type == eSpellType::Follow && this->m_target == nullptr)
	{
		//Oh well sombody tried to launch a speel that follows a targets but provided no target :?
		//Changing to skillshot
		this->m_type = eSpellType::SkillShot;
		//As there is sopoused to be a target, make the skill shot go to a random spot
		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_int_distribution<int> did(-1, 1);
		int xDir = did(dre);
		int yDir = 0;
		if (xDir == 0)
		{
			do
			{
				yDir = did(dre);
			} while (yDir == 0);
		}
		this->m_direction = Vector2((float)xDir, (float)yDir);
	}
}
