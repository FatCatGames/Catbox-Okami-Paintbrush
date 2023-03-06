#pragma once
#include "Assets/MeshData.h"
#include "Debugging\DebugShapes.h"

class DebugDrawer
{
public:
	DebugDrawer() = default;
	~DebugDrawer() = default;
	static void DrawLine(const Vector3f& aFrom, const Vector3f& aTo, const Color& aColor = Color{ 1,1,1,1 }, float aAliveTime = 0);
	static void DrawCube(const Vector3f& aPosition, const Vector3f& aScale, const Vector3f& aRotation = { 1,1,1 }, const Color& aColor = Color{ 1,1,1,1 }, const Vector3f& aCustomPivot = {0,0,0}, const Transform& aOffsetTransfrom = Transform());
	static void DrawSphere(const Vector3f& aPosition, float aRadius, const Color& aColor = Color{ 1,1,1,1 });
};