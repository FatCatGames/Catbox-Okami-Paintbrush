#pragma once
class RigidBody;
class Tree : public Component
{
public:
	void Slash(const Vector3f& anIntersectionPos, const Vector3f& anIntersectionDir);

private:
};