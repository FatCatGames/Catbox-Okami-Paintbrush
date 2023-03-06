#pragma once
#include "Composite.h"
#include <cassert>

class SelectorRandom : public Composite
{
public:
    void Initialize() override;
    Status Update() override;

private:
    std::vector<int> mySelection;
};
