#pragma once
#include "EditorCommand.h"

template<typename T>
class ImGuiValueCommand : public EditorCommand
{
public:
	ImGuiValueCommand<T>(T* aProperty, T aTranslation);
	~ImGuiValueCommand<T>() = default;

private:
	struct Command 
	{
		T* property;
		T translation;
	} myCommand;

	inline void Redo() override;
	inline void Undo() override;
};

template<typename T>
inline ImGuiValueCommand<T>::ImGuiValueCommand(T* aProperty, T aTranslation)
{
	myCommand.property = aProperty;
	myCommand.translation = aTranslation;
	print("Changed value from " + std::to_string(*aProperty - aTranslation) + " to " + std::to_string(*aProperty));
}

template<typename T>
inline void ImGuiValueCommand<T>::Redo()
{
	*myCommand.property += myCommand.translation;
}

template<typename T>
inline void ImGuiValueCommand<T>::Undo()
{
	*myCommand.property -= myCommand.translation;
}
