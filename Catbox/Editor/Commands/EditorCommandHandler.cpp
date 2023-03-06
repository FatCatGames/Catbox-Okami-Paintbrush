#include "stdafx.h"
#include "EditorCommandHandler.h"
#include "EditorCommand.h"

int EditorCommandHandler::CommandsSinceSave = 0;
int EditorCommandHandler::CommandIndex = 0;
std::vector<std::shared_ptr<EditorCommand>> EditorCommandHandler::CommandHistory;

void EditorCommandHandler::AddCommand(const std::shared_ptr<EditorCommand> aCommand)
{
	CommandHistory.erase(CommandHistory.begin() + CommandIndex, CommandHistory.end());
	CommandHistory.push_back(aCommand);
	++CommandIndex;
	++CommandsSinceSave;
}

void EditorCommandHandler::Undo()
{
	if (CommandIndex == 0) return;
	CommandHistory[--CommandIndex]->Undo();
}

void EditorCommandHandler::Redo()
{
	if (CommandIndex == CommandHistory.size()) return;
	CommandHistory[CommandIndex++]->Redo();
}
