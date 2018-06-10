#pragma once
#include "Entity.hpp"
class TileColision : public Entity
{
public:
	TileColision(std::string identifier, Rectangle colisionRect, unsigned int frameNum = 0);
	virtual ~TileColision();

	virtual void update(const Rectangle cameraRect, SOUND::SoundManager& sm) override final;
};

