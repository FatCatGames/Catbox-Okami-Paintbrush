#pragma once
#include "EditorCommand.h"
#include "Vector3.hpp"
#include "GameObjects/Transform.h"

class TransformCommand : public EditorCommand
{
public:
	TransformCommand(Transform* aTransform, Transform::Operation anOperation, Transform::Mode aMode, Catbox::Vector3<float> aTranslation);
	TransformCommand(std::vector<Transform*> someTransforms, Transform::Operation anOperation, Transform::Mode aMode, Catbox::Vector3<float> aTranslation);
	~TransformCommand() = default;
	
private:
	struct Command 
	{
		std::vector<int> objectIds;
		Transform::Operation operation;
		Transform::Mode mode;
		Catbox::Vector3<float> translation;
	} myCommand;

	void Redo() override;
	void Undo() override;
	void ExecuteCommand(float aModifier);
};