#pragma once
#include "Entity.hpp"

class SpellEntity : public Entity
{
public:
	enum class eSpellType
	{
		Follow, SkillShot
	};

	SpellEntity();
	virtual ~SpellEntity();

	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override final;

	virtual bool isColliding(const Entity& other) const;
	virtual void collision(Entity& other);
	void Instanciate(const float damage, const Entity* const owner, const  Entity* const target, const eSpellType type, const Vector2 dir, const std::string identifier, const Rectangle colissionVolume, const DWORD frameTimeMS = 1000, const int frameNumStart = 0, const int frameNumEnd = 0);

private:
	DWORD m_lastTimeAnimUpdate;
	DWORD m_frameTime;
	eSpellType m_type;
	int m_frameNumEnd;
	const Entity* m_owner;
	const Entity* m_target;
	Vector2 m_direction;
	Vector2 m_startPosition;
	bool m_firstUpdate;
};

