#pragma once

#include "Entity.hpp"

class ItemStack : public Entity
{
public:
	ItemStack(std::string identifier);
	~ItemStack();

	virtual void update() override;
};

