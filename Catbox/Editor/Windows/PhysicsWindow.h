#pragma once
#include "EditorWindow.h"

class PhysicsWindow : public EditorWindow
{
public:
	PhysicsWindow();

private:
	void Render() override;
	std::shared_ptr<CollisionManager> myCollisionManager;
};

