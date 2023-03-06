#pragma once
#include "EditorCommand.h"


class AddRemoveComponentCommand : public EditorCommand
{
public:
	enum class Operation
	{
		Add,
		Remove
	};

	AddRemoveComponentCommand(Component* aComponent, Operation anOperation);
	~AddRemoveComponentCommand() = default;

private:
	struct Command
	{
		string componentData;
		Operation operation;
		GameObject* object;
	} myCommand;

	void Redo() override;
	void Undo() override;
	void AddComponent();
	void RemoveComponent();
};