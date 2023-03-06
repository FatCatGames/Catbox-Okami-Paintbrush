#include "stdafx.h"
#include "CreateDeleteObjectCommand.h"
#include "Editor\Windows\SceneHierarchyWindow.h"

CreateDeleteObjectCommand::CreateDeleteObjectCommand(GameObject* anObject, Operation anOperation)
{
	myCommand.objectData = anObject->GetObjectData();
	myCommand.operation = anOperation;
	myCommand.objectId = anObject->GetObjectInstanceID();
	myCommand.parentId = anObject->GetTransform()->GetParent() ? anObject->GetTransform()->GetParent()->GetGameObject()->GetObjectInstanceID() : -1;
}

void CreateDeleteObjectCommand::Redo()
{
	if (myCommand.operation == Operation::Create) CreateObject();
	else DeleteObject();
}

void CreateDeleteObjectCommand::Undo()
{
	if (myCommand.operation == Operation::Create) DeleteObject();
	else CreateObject();
 }

void CreateDeleteObjectCommand::CreateObject()
{
	GameObject* object = Engine::GetInstance()->GetGameObjectFactory()->CreateGameObjectWithId(myCommand.objectId);
	auto parentObj = Engine::GetGameObject(myCommand.parentId);
	if (parentObj) 
	{
		object->GetTransform()->SetParent(parentObj->GetTransform());
	}
	object->LoadObjectData(myCommand.objectData);
	
	auto hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
	if (hierarchy)
	{
		//if (hierarchy->GetSelectedObjects().empty()) 
		{
			hierarchy->AddSelectedObject(object);
			Editor::GetInstance()->GetGizmos().AddSelectedObject(object);
			object->AddSelected();
		}
	}
}

void CreateDeleteObjectCommand::DeleteObject()
{
	GameObject* object = Engine::GetInstance()->GetGameObject(myCommand.objectId);
	if (object) 
	{
		auto hierarchy = Editor::GetInstance()->GetWindowHandler().GetWindowOfType<SceneHierarchyWindow>(WindowType::SceneHierarchy);
		if (hierarchy)
		{
			hierarchy->RemoveSelectedObject(object);
			Editor::GetInstance()->GetGizmos().RemoveSelectedObject(object);
			object->Deselect();
		}

		object->Destroy();
		object = nullptr;
	}
}
