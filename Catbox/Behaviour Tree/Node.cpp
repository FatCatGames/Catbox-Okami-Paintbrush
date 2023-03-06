#include "stdafx.h"
#include "Node.h"

Node::Status Node::Run()
{
    if (myStatus != Status::Running)
    {
        Initialize();
    }

    myStatus = Update();

    if (myStatus != Status::Running)
    {
        Terminate(myStatus);
    }

    return myStatus;
}

bool Node::IsSuccess() const
{
    return myStatus == Status::Success;
}

bool Node::IsFailure() const
{
    return myStatus == Status::Failure;
}

bool Node::IsRunning() const
{
    return myStatus == Status::Running;
}

bool Node::IsTerminated() const
{
    return IsSuccess() || IsFailure();
}

void Node::Reset()
{
    myStatus = Status::Invalid;
}
