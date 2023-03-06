#pragma once
#include "EditorCommand.h"
#include "Vector4.hpp"

template<typename T>
class Vector4Command : public EditorCommand
{
public:
	Vector4Command<T>(Catbox::Vector4<T>* aProperty, Catbox::Vector4<T> aTranslation);
	~Vector4Command<T>() = default;
	
private:
	struct Command 
	{
		Catbox::Vector4<T>* property;
		Catbox::Vector4<T> translation;
	} myCommand;

	inline void Redo() override;
	inline void Undo() override;
};

template<typename T>
inline Vector4Command<T>::Vector4Command(Catbox::Vector4<T>* aProperty, Catbox::Vector4<T> aTranslation)
{
	myCommand.property = aProperty;
	myCommand.translation = aTranslation;
}

template<typename T>
inline void Vector4Command<T>::Redo()
{
	*myCommand.property += myCommand.translation;
}

template<typename T>
inline void Vector4Command<T>::Undo()
{
	*myCommand.property -= myCommand.translation;
}
