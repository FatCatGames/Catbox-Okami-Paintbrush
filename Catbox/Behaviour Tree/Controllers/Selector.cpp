#include "stdafx.h"
#include "Selector.h"

void Selector::Initialize()
{
    myIndex = 0;
}

Node::Status Selector::Update()
{
    assert(Composite::HasChildren() && "Selector has no children");

    while (myIndex < Composite::myChildren.size()) {
        auto status = (Composite::myChildren[myIndex])->Run();

        if (status != Status::Failure) {
            return status;
        }

        myIndex++;
    }

    return Status::Failure;
}

