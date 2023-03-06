#include "stdafx.h"
#include "Inverter_Decorator.h"

Node::Status Inverter::Update()
{
    Node::Status status = myChild->Run();

    if (status == Status::Success)
    {
        return Status::Failure;
    }
    else if (status == Status::Failure)
    {
        return Status::Success;
    }

    return status;
}
