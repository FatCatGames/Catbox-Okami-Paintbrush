#pragma once
#include "Composite.h"
#include <cassert>

class Selector : public Composite
{
public:
    void Initialize() override;
    Status Update() override;
private:
    int myIndex = 0;
};
