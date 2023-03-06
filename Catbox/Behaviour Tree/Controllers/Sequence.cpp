#include "stdafx.h"
#include "Sequence.h"


void Sequence::Initialize()
{
    myIndex = 0;
}

Node::Status Sequence::Update()
{
    assert(Composite::HasChildren() && "Sequence has no chirldren");

    while (myIndex < Composite::myChildren.size()) {
        auto status = (Composite::myChildren[myIndex])->Run();

        if (status != Node::Status::Success)
        {
            return status;
        }

        myIndex++;
    }
    
    return Node::Status::Success;
}

