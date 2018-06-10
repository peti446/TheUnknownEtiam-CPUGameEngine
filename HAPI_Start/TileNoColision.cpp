#include "TileNoColision.hpp"


TileNoColision::TileNoColision(std::string identifier, Rectangle colisionRect, unsigned int frameNum) : Entity(identifier, colisionRect, frameNum)
{
	this->m_side = eSide::NoColision;
}


TileNoColision::~TileNoColision()
{
}

void TileNoColision::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;
}

