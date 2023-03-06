#pragma once
#include "EditorCommand.h"

class CreateDeleteObjectCommand : public EditorCommand
{
public:
	enum class Operation 
	{
		Create,
		Delete
	};

	CreateDeleteObjectCommand(GameObject* anObject, Operation anOperation);
	~CreateDeleteObjectCommand() = default;

private:
	struct Command
	{
		string objectData;
		Operation operation;
		int objectId;
		int parentId;
	} myCommand;

	void Redo() override;
	void Undo() override;
	void CreateObject();
	void DeleteObject();
};