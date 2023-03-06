#pragma once
#include "../Node.h"
#include <vector>
#include <memory>

class Composite : public Node
{
public:
    Composite();
    virtual ~Composite() {}

    void AddChild(std::shared_ptr<Node> aChild);
    bool HasChildren() const;

protected:
    std::vector<std::shared_ptr<Node>> myChildren;
};