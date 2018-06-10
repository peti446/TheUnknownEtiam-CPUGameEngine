#pragma once
#include "Entity.hpp"
class TileNoColision : public Entity
{
public:
	TileNoColision(std::string identifier, Rectangle colisionRect, unsigned int frameNum = 0);
	virtual ~TileNoColision();

	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override final;
};

