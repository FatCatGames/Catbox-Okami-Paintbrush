#include "stdafx.h"
#include "Repeater_Decorator.h"


Repeater::Repeater(int aLimit)
{
    limit = aLimit;
}

void Repeater::Initialize()
{

}

Node::Status Repeater::Update()
{
    Node::Status status = myChild->Run();

    if (status == Status::Success) {
        return Status::Success;
    }

    if (limit > 0 && ++counter == limit) {
        return Status::Success;
    }

    return Status::Running;
}

