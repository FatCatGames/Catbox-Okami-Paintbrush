#pragma once
#include "Controllers/Blackboard.h"
class BehaviorTree : public Node
{
public:
    BehaviorTree();
    BehaviorTree(const std::shared_ptr<Node>& rootNode);

    Status Update();
    void SetRoot(const std::shared_ptr<Node>& node);
    std::shared_ptr<Blackboard> GetBlackboard() const;

private:
    std::shared_ptr<Node> myRoot = nullptr;
    std::shared_ptr<Blackboard> myBlackboard = nullptr;
};