#pragma once
#include <vector>
#include <functional>
#include <string>

class State
{
	struct StateTransition
	{
		State* nextState;
		bool CanTransition()
		{
			return condition();
		}
		std::function<bool()> condition;
	};


public:
	~State();
	State* CheckTransitions();
	virtual void Enter();
	virtual void Exit();
	void AddTransition(State* aToState, std::function<bool()> aCondition);
	virtual void Update();
	virtual void FixedUpdate();
	std::string GetStateMessage() const { return myStateMessage; };
	void SetStateMessage(std::string aMessage) { myStateMessage = aMessage; };

private:
	std::string myStateMessage = "UnDefined";
	std::vector<StateTransition*> myTransitions;

};