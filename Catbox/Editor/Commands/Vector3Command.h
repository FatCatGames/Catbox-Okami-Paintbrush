#pragma once
#include "EditorCommand.h"
#include "Vector3.hpp"

template<typename T>
class Vector3Command : public EditorCommand
{
public:
	Vector3Command<T>(Catbox::Vector3<T>* aProperty, Catbox::Vector3<T> aTranslation);
	~Vector3Command<T>() = default;
	
private:
	struct Command 
	{
		Catbox::Vector3<T>* property;
		Catbox::Vector3<T> translation;
	} myCommand;

	inline void Redo() override;
	inline void Undo() override;
};

template<typename T>
inline Vector3Command<T>::Vector3Command(Catbox::Vector3<T>* aProperty, Catbox::Vector3<T> aTranslation)
{
	myCommand.property = aProperty;
	myCommand.translation = aTranslation;
}

template<typename T>
inline void Vector3Command<T>::Redo()
{
	*myCommand.property += myCommand.translation;
}

template<typename T>
inline void Vector3Command<T>::Undo()
{
	*myCommand.property -= myCommand.translation;
}
