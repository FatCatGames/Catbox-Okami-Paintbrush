#include "stdafx.h"
#include "BehaviourTree.h"

BehaviorTree::BehaviorTree()
{
	myBlackboard = std::make_shared<Blackboard>();
}

BehaviorTree::BehaviorTree(const std::shared_ptr<Node>& rootNode)
{
	myRoot = rootNode; 
}

Node::Status BehaviorTree::Update()
{
	return myRoot->Run();
}

void BehaviorTree::SetRoot(const std::shared_ptr<Node>& node)
{
	myRoot = node;
}

std::shared_ptr<Blackboard> BehaviorTree::GetBlackboard() const
{
	return myBlackboard;
}
