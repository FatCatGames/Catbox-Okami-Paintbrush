#include "stdafx.h"
#include "AddRemoveComponentCommand.h"

AddRemoveComponentCommand::AddRemoveComponentCommand(Component* aComponent, Operation anOperation)
{
	myCommand.componentData = aComponent->GetComponentData();
	myCommand.object = &aComponent->GetGameObject();
	myCommand.operation = anOperation;
}
