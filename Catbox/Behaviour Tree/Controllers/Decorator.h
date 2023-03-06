#pragma once
#include "..\Node.h"
#include <cassert>
#include "Blackboard.h"

class Decorator : public Node
{
public:
    virtual ~Decorator() {}
    Decorator() {};
    Decorator(std::shared_ptr<Blackboard> aBlackboard);

    void SetChild(std::shared_ptr<Node> aNode);
    bool HasChild() const;

protected:
    std::shared_ptr<Node> myChild = nullptr;
    std::shared_ptr<Blackboard> myBlackboard;
};