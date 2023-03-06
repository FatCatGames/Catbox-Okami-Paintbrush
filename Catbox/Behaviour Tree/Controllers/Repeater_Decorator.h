#pragma once
#include "../Node.h"
#include "Decorator.h"
#include <cassert>

class Repeater : public Decorator
{
public:
    Repeater(int aLimit = 0);
    void Initialize() override;
    Status Update() override;
protected:
    int limit;
    int counter = 0;
};
