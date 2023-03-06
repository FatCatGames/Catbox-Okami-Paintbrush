#include "stdafx.h"
#include "Composite.h"

Composite::Composite()
{


}

void Composite::AddChild(std::shared_ptr<Node> aChild)
{
	myChildren.push_back(aChild);
}

bool Composite::HasChildren() const
{ 
	return !myChildren.empty();
}
