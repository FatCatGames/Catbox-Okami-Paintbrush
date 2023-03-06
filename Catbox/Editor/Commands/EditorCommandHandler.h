#pragma once

class EditorCommand;
class EditorCommandHandler
{
public:
	EditorCommandHandler() = default;
	~EditorCommandHandler() = default;

	static void AddCommand(const std::shared_ptr<EditorCommand> aCommand);
	static void Undo();
	static void Redo();
	static inline void ClearCommandSinceSaveCount() { CommandsSinceSave = 0; }
	static inline int GetCommandCountSinceSave() { return CommandsSinceSave; }

private:
	static int CommandIndex;
	static std::vector<std::shared_ptr<EditorCommand>> CommandHistory;
	static int CommandsSinceSave;
};