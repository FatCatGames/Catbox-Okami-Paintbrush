#pragma once
#include "CommonUtilities\Matrix4x4.hpp"
#include "CommonUtilities\Vector.h"

struct DebugLine
{
	Vector4f from;
	Vector4f to;
	Color color;
	float aliveTime = 0;
};

struct DebugCube
{
	Catbox::Matrix4x4<float> transform;
	Color color;
};