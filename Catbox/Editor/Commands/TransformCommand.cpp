#include "stdafx.h"
#include "TransformCommand.h"


TransformCommand::TransformCommand(Transform* aTransform, Transform::Operation anOperation, Transform::Mode aMode, Catbox::Vector3<float> aTranslation)
{
	myCommand.translation = aTranslation;
	myCommand.objectIds.push_back(aTransform->GetGameObject()->GetObjectInstanceID());
	myCommand.operation = anOperation;
	myCommand.mode = aMode;
}

TransformCommand::TransformCommand(std::vector<Transform*> someTransforms, Transform::Operation anOperation, Transform::Mode aMode, Catbox::Vector3<float> aTranslation)
{
	myCommand.translation = aTranslation;
	for (size_t i = 0; i < someTransforms.size(); i++)
	{
		myCommand.objectIds.push_back(someTransforms[i]->GetGameObject()->GetObjectInstanceID());
	}
	myCommand.operation = anOperation;
	myCommand.mode = aMode;
}

void TransformCommand::ExecuteCommand(float aModifier)
{
	for (auto& objectId : myCommand.objectIds)
	{
		GameObject* object = Engine::GetGameObject(objectId);
		if (!object)
		{
			printerror("Tried to execute a command on a transform that has been deleted!");
			continue;
		}
		Transform* transform = object->GetTransform();
		if (myCommand.mode == Transform::Mode::World)
		{
			if (myCommand.operation == Transform::Operation::Translate)
			{
				transform->Translate(aModifier * myCommand.translation);
			}
			else if (myCommand.operation == Transform::Operation::Rotate)
			{
				transform->Rotate(aModifier * myCommand.translation);
			}
			else //Scale
			{
				transform->SetWorldScale(transform->worldScale() + (aModifier * myCommand.translation));
			}
		}
		else //Local
		{
			if (myCommand.operation == Transform::Operation::Translate)
			{
				transform->SetLocalPos(transform->localPos() + (aModifier * myCommand.translation));
			}
			else if (myCommand.operation == Transform::Operation::Rotate)
			{
				transform->SetLocalRot(transform->localRot() + (aModifier * myCommand.translation));

			}
			else //Scale
			{
				transform->SetLocalScale(transform->localScale() + (aModifier * myCommand.translation));
			}
		}
	}
}

void TransformCommand::Redo()
{
	ExecuteCommand(1);
}


void TransformCommand::Undo()
{
	ExecuteCommand(-1);
}
