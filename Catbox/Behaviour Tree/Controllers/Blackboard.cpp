#include "stdafx.h"
#include "Blackboard.h"

bool Blackboard::GetBool(const std::string& aKey)
{
    if (myBools.find(aKey) == myBools.end())
    {
        myBools[aKey] = false;
    }
    return myBools[aKey];
}

int Blackboard::GetInt(const std::string& aKey)
{
    if (myInts.find(aKey) == myInts.end())
    {
        myInts[aKey] = 0;
    }
    return myInts[aKey];
}

float Blackboard::GetFloat(const std::string& aKey)
{
    if (myFloats.find(aKey) == myFloats.end())
    {
        myFloats[aKey] = 0.0f;
    }
    return myFloats[aKey];
}

std::string Blackboard::GetString(const std::string& aKey)
{
    if (myStrings.find(aKey) == myStrings.end())
    {
        myStrings[aKey] = "";
    }
    return myStrings[aKey];
}

Vector3f Blackboard::GetVector3f(const std::string& aKey)
{
    if (myVectors.find(aKey) == myVectors.end())
    {
        myVectors[aKey] = {0,0,0};
    }
    return myVectors[aKey];
}
