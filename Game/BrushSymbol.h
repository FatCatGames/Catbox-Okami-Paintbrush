#pragma once

enum BrushTarget
{
	Default,
	Sky,
	Water
};

struct BrushSymbol
{
	std::string name;
	BrushTarget target;
	float minX;
	float minY;
	float maxX;
	float maxY;
};