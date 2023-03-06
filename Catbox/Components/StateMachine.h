#pragma once
#include "../ComponentTools/State.h"
#include "../GameObjects/Component.h"
#include "ComponentTools/Event.h"

class StateMachine : public Component
{
public:
	StateMachine() = default;
	~StateMachine();

	void AddState(State* aState);
	void AddTransition(State* aFromState, State* aToState, std::function<bool()> aCondition);
	void AddTransitionFromAnyState(State* aToState, std::function<bool()> aCondition);
	void SetState(State* aState);
	void Update() override;
	void FixedUpdate() override;
	void AddListener(Listener aEvent);
	void AddListener(ListenerWithParameter aEvent);
	State* GetCurrentState();

private:
	Event myEvent;
	State* myCurrentState = nullptr;
	void Save(rapidjson::Value& aComponentData) override;
	std::vector<State*> myStates;
	std::vector<State*> myAlwaysCheckStates; //states that can be transitioned to from any other state
};