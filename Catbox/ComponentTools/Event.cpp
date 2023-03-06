#include "stdafx.h"
#include "Event.h"


void Event::AddListener(Listener& aListener)
{
	aListener.listenerId = static_cast<int>(myListeners.size());
	myListeners.push_back(aListener);
}

void Event::RemoveListener(Listener& aListener)
{
	myListeners.erase(myListeners.begin() + aListener.listenerId);
}

void Event::RemoveListener(ListenerWithParameter& aListener)
{
	myListenersWithParameter.erase(myListenersWithParameter.begin() + aListener.listenerId);
}
void Event::RemoveListener(ListenerWithParameterAndID& aListener)
{
	myListenersWithParameterAndID.erase(myListenersWithParameterAndID.begin() + aListener.listenerId);
}


void Event::AddListener(ListenerWithParameter& aListener)
{
	aListener.listenerId = static_cast<int>(myListenersWithParameter.size());
	myListenersWithParameter.push_back(aListener);
}

void Event::AddListener(ListenerWithParameterAndID& aListener)
{
	aListener.listenerId = static_cast<int>(myListenersWithParameterAndID.size());
	myListenersWithParameterAndID.push_back(aListener);
}

void Event::Invoke(void* aPointer)
{
	for (size_t i = 0; i < myListenersWithParameter.size(); i++)
	{
		myListenersWithParameter[i].action(aPointer);
	}
}

void Event::Invoke(void* aPointer, unsigned short aId)
{
	for (size_t i = 0; i < myListenersWithParameterAndID.size(); i++)
	{
		myListenersWithParameterAndID[i].action(aPointer, aId);
	}
}

void Event::Invoke()
{
	for (size_t i = 0; i < myListeners.size(); i++)
	{
		myListeners[i].action();
	}
}
