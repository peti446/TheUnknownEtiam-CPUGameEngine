#include "TileColision.hpp"



TileColision::TileColision(std::string identifier, Rectangle colisionRect, unsigned int frameNum) : Entity(identifier, colisionRect, frameNum)
{
	this->m_side = eSide::DEFAULT;
}


TileColision::~TileColision()
{
}

void TileColision::update(const Rectangle cameraRect, SOUND::SoundManager& sm)
{
	if (!this->isActive())
		return;
}
