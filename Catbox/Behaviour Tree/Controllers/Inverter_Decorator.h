#pragma once
#include "..\Node.h"
#include "Decorator.h"
#include <cassert>

class Inverter : public Decorator
{
public:
    Status Update() override;
};
