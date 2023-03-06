#include "stdafx.h"
#include "State.h"

State::~State()
{
	for (int i = myTransitions.size() - 1; i >= 0; i--)
	{
		delete myTransitions[i];
	}
}

State* State::CheckTransitions()
{
	for (size_t i = 0; i < myTransitions.size(); i++)
	{
		if (myTransitions[i]->CanTransition()) 
		{
			return myTransitions[i]->nextState;
		}
	}
	return nullptr;
}

void State::Enter()
{
}

void State::Exit()
{
}

void State::AddTransition(State* aToState, std::function<bool()> aCondition)
{
	StateTransition* transition = new StateTransition();
	transition->condition = aCondition;
	transition->nextState = aToState;
	myTransitions.push_back(transition);
}

void State::Update()
{
}

void State::FixedUpdate()
{
}
