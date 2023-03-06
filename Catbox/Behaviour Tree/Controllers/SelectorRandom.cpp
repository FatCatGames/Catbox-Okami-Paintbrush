#include "stdafx.h"
#include "SelectorRandom.h"

void SelectorRandom::Initialize()
{
    for(int i = 0; i < Composite::myChildren.size();i++)
    {
        bool valueSame = true;
        while(valueSame)
        {
            int value = Catbox::GetRandom<int>(0, Composite::myChildren.size() - 1);
            valueSame = false;
            for (int x = 0; x < mySelection.size(); x++)
            {
                if (value == mySelection[x])
                {
                    valueSame = true;
                }
            }
            mySelection.push_back(value);
        }
    }
}

Node::Status SelectorRandom::Update()
{
    assert(Composite::HasChildren() && "RandSelector has no children");

    for(int i = 0; i < mySelection.size();i++)
    {
        auto status = (Composite::myChildren[mySelection[i]])->Run();

        if (status != Status::Failure) {
            return status;
        }

    }

    return Status::Failure;
}

