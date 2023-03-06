#include "stdafx.h"
#include "../Behaviour Tree/Controllers/Blackboard.h"
#include "../Leafes/Leaf.h"


Leaf::Leaf(std::shared_ptr<Blackboard> aBlackboard)
{
	myBlackboard = aBlackboard;
}

Leaf::~Leaf()
{
}
