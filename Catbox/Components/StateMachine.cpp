#include "stdafx.h"
#include "Statemachine.h"


StateMachine::~StateMachine() 
{
	for (int i = myStates.size() - 1; i >= 0; i--)
	{
		if (myStates[i]) 
		{
			delete myStates[i];
		}
	}
}

void StateMachine::AddState(State* aState)
{
	if (!myCurrentState) myCurrentState = aState;
	myStates.push_back(aState);
}

void StateMachine::AddTransition(State* aFromState, State* aToState, std::function<bool()> aCondition)
{
	aFromState->AddTransition(aToState, aCondition);
	for (size_t i = 0; i < myAlwaysCheckStates.size(); i++)
	{
		myStates[i]->AddTransition(myAlwaysCheckStates[i], aCondition);
	}
}

void StateMachine::AddTransitionFromAnyState(State* aToState, std::function<bool()> aCondition)
{
	for (size_t i = 0; i < myStates.size(); i++)
	{
		if (myStates[i] == aToState) continue;
		myStates[i]->AddTransition(aToState, aCondition);
	}
	myAlwaysCheckStates.push_back(aToState);
}

void StateMachine::SetState(State* aState)
{
	if (myCurrentState != nullptr)
	{
		myCurrentState->Exit();
		myCurrentState = aState;
		myCurrentState->Enter();
	}
}


void StateMachine::Update()
{
	if (!myCurrentState) return;
	myCurrentState->Update();
	State* newState = myCurrentState->CheckTransitions();
	if (newState) 
	{
		myCurrentState->Exit();
		myCurrentState = newState;
		myEvent.Invoke(&myCurrentState->GetStateMessage());
		myCurrentState->Enter();
	}
}

void StateMachine::FixedUpdate()
{
//	myCurrentState->FixedUpdate(aFixedDeltaTime);
}

void StateMachine::AddListener(Listener aListener)
{
	myEvent.AddListener(aListener);
}
void StateMachine::AddListener(ListenerWithParameter aListener)
{
	myEvent.AddListener(aListener);
}

State* StateMachine::GetCurrentState() 
{
	return myCurrentState;
}

void StateMachine::Save(rapidjson::Value& /*aComponentData*/)
{

}
