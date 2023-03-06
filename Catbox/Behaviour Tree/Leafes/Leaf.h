#pragma once
#include "../Node.h"

class Leaf : public Node
{
public:
    Leaf(std::shared_ptr<Blackboard> aBlackboard);
    virtual ~Leaf();
    virtual Status Update() = 0;

protected:
    std::shared_ptr<Blackboard> myBlackboard;

};
