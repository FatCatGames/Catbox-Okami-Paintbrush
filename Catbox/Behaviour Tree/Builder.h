#pragma once
#include <cassert>
#include "Controllers/Decorator.h"
#include "Controllers/Composite.h"
#include "Behaviour Tree/BehaviourTree.h"

template <class Parent>
class DecoratorBuilder;

template <class Parent>
class CompositeBuilder
{
public:
    CompositeBuilder(Parent* parent, Composite* node) : myParent(parent), myNode(node) {}

    template <class NodeType, typename... Args>
    CompositeBuilder<Parent> leaf(Args... args)
    {
        auto child = std::make_shared<NodeType>((args)...);
        myNode->AddChild(child);
        return *this;
    }

    template <class CompositeType, typename... Args>
    CompositeBuilder<CompositeBuilder<Parent>> composite(Args... args)
    {
        auto child = std::make_shared<CompositeType>((args)...);
        myNode->AddChild(child);
        return CompositeBuilder<CompositeBuilder<Parent>>(this, (CompositeType*)child.get());
    }

    template <class DecoratorType, typename... Args>
    DecoratorBuilder<CompositeBuilder<Parent>> decorator(Args... args)
    {
        auto child = std::make_shared<DecoratorType>((args)...);
        myNode->AddChild(child);
        return DecoratorBuilder<CompositeBuilder<Parent>>(this, (DecoratorType*)child.get());
    }

    Parent& end()
    {
        return *myParent;
    }

private:
    Parent* myParent;
    Composite* myNode;
};

template <class Parent>
class DecoratorBuilder
{
public:
    DecoratorBuilder(Parent* parent, Decorator* node) : myParent(parent), myNode(node) {}

    template <class NodeType, typename... Args>
    DecoratorBuilder<Parent> leaf(Args... args)
    {
        auto child = std::make_shared<NodeType>((args)...);
        myNode->SetChild(child);
        return *this;
    }

    template <class CompositeType, typename... Args>
    CompositeBuilder<DecoratorBuilder<Parent>> composite(Args... args)
    {
        auto child = std::make_shared<CompositeType>((args)...);
        myNode->SetChild(child);
        return CompositeBuilder<DecoratorBuilder<Parent>>(this, (CompositeType*)child.get());
    }

    template <class DecoratorType, typename... Args>
    DecoratorBuilder<DecoratorBuilder<Parent>> decorator(Args... args)
    {
        auto child = std::make_shared<DecoratorType>((args)...);
        myNode->SetChild(child);
        return DecoratorBuilder<DecoratorBuilder<Parent>>(this, (DecoratorType*)child.get());
    }

    Parent& end()
    {
        return *myParent;
    }

private:
    Parent* myParent;
    Decorator* myNode;
};


class Builder
{
public:
    template <class NodeType, typename... Args>
    Builder leaf(Args... args)
    {
        myRoot = std::make_shared<NodeType>((args)...);
        return *this;
    }

    template <class CompositeType, typename... Args>
    CompositeBuilder<Builder> composite(Args... args)
    {
        myRoot = std::make_shared<CompositeType>((args)...);
        return CompositeBuilder<Builder>(this, (CompositeType*)myRoot.get());
    }

    template <class DecoratorType, typename... Args>
    DecoratorBuilder<Builder> decorator(Args... args)
    {
        myRoot = std::make_shared<DecoratorType>((args)...);
        return DecoratorBuilder<Builder>(this, (DecoratorType*)myRoot.get());
    }

    std::shared_ptr<Node> build()
    {
        assert(myRoot != nullptr && "The Behavior Tree is empty!");
        auto tree = std::make_shared<BehaviorTree>();
        tree->SetRoot(myRoot);
        return tree;
    }

private:
    std::shared_ptr<Node> myRoot;
};

