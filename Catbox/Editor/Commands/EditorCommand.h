#pragma once


class EditorCommand 
{
public:
	EditorCommand() = default;
	virtual ~EditorCommand() = default;

protected:
	friend class EditorCommandHandler;
	virtual void Redo() {};
	virtual void Undo() {};
};