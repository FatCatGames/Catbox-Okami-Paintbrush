#include "stdafx.h"
#include "Decorator.h"


Decorator::Decorator(std::shared_ptr<Blackboard> aBlackboard)
{
    myBlackboard = aBlackboard;
}

void Decorator::SetChild(std::shared_ptr<Node> aNode)
{
    myChild = aNode;
}

bool Decorator::HasChild() const
{
    return myChild != nullptr;
}
