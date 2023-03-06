#include "stdafx.h"
#include "DebugDrawer.h"
#include "Graphics\Rendering\Renderers\DebugRenderer.h"


void DebugDrawer::DrawLine(const Vector3f& aFrom, const Vector3f& aTo, const Color& aColor, float aAliveTime)
{
	DebugLine line;
	line.color = aColor;
	line.from = Vector4f(aFrom.x, aFrom.y, aFrom.z, 1);
	line.to = Vector4f(aTo.x, aTo.y, aTo.z, 1);
	line.aliveTime = aAliveTime;
	Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(line);
}

void DebugDrawer::DrawCube(const Vector3f& aPosition, const Vector3f& aScale, const Vector3f& aRotation, const Color& aColor, const Vector3f& aCustomPivot, const Transform& aOffsetTransfrom)
{
	DebugCube box;
	box.transform(4, 1) = aCustomPivot.x;
	box.transform(4, 2) = aCustomPivot.y;
	box.transform(4, 3) = aCustomPivot.z;

	Catbox::Matrix4x4<float> rotationMatrix = Catbox::Matrix4x4<float>();
	rotationMatrix = rotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(aRotation.z)) * rotationMatrix;
	rotationMatrix = rotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(aRotation.y)) * rotationMatrix;
	rotationMatrix = rotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(aRotation.x)) * rotationMatrix;

	Vector4f scale = { aScale.x, aScale.y, aScale.z, 1 };
	Catbox::Matrix4x4<float> scaleMatrix;
	scaleMatrix = scaleMatrix * scale;

	Catbox::Matrix4x4<float> aTranslation = Catbox::Matrix4x4<float>();
	aTranslation(4, 1) = aPosition.x;
	aTranslation(4, 2) = aPosition.y;
	aTranslation(4, 3) = aPosition.z;

	Catbox::Matrix4x4<float> myWorldTransform = scaleMatrix * (box.transform * rotationMatrix) * aTranslation;

	box.transform = myWorldTransform;
	box.color = aColor;

	Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(box);
}

void DebugDrawer::DrawSphere(const Vector3f& aPosition, float aRadius, const Color& aColor)
{
	DebugDrawer::DrawLine(Vector3f(aPosition.x - aRadius, aPosition.y, aPosition.z), Vector3f(aPosition.x + aRadius, aPosition.y, aPosition.z), aColor);
	DebugDrawer::DrawLine(Vector3f(aPosition.x, aPosition.y - aRadius, aPosition.z), Vector3f(aPosition.x, aPosition.y + aRadius, aPosition.z), aColor);
	DebugDrawer::DrawLine(Vector3f(aPosition.x, aPosition.y, aPosition.z - aRadius), Vector3f(aPosition.x, aPosition.y, aPosition.z + aRadius), aColor);
}
