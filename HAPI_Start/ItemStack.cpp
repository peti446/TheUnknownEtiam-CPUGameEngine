#include "ItemStack.hpp"



ItemStack::ItemStack(std::string identidier) : Entity(identidier)
{
}


ItemStack::~ItemStack()
{
}

void ItemStack::update()
{
	this->applyGraity();
}
