#pragma once

struct NodeGraphEdge
{
	size_t EdgeId;
	size_t FromUID;
	size_t ToUID;

	ImVec4 Color;
	float Thickness = 1.0f;
};
