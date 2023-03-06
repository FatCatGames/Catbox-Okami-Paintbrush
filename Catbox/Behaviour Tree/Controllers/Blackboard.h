#pragma once
#include "../Node.h"
#include <string>
#include <unordered_map>
#include "Components\Physics\CharacterController.h"

class Blackboard
{
public:
    void SetBool(const std::string& aKey, const bool &aValue) { myBools[aKey] = aValue; }
    bool GetBool(const std::string& aKey);
    bool HasBool(const std::string& aKey) const { return myBools.find(aKey) != myBools.end(); }

    void SetInt(const std::string& aKey, const int& aValue) { myInts[aKey] = aValue; }
    int GetInt(const std::string& aKey);
    bool HasInt(const std::string& aKey) const { return myInts.find(aKey) != myInts.end(); }

    void SetFloat(const std::string& aKey, const float& aValue) { myFloats[aKey] = aValue; }
    float GetFloat(const std::string& aKey);
    bool HasFloat(const std::string& aKey) const { return myFloats.find(aKey) != myFloats.end(); }

    void SetString(const std::string& aKey, const std::string& aValue) { myStrings[aKey] = aValue; }
    std::string GetString(const std::string& aKey);
    bool HasString(const std::string& aKey) const { return myStrings.find(aKey) != myStrings.end(); }

    void SetVector3f(const std::string& aKey, const Vector3f& aValue) { myVectors[aKey] = aValue; }
    Vector3f GetVector3f(const std::string& aKey);
    bool HasVector3f(const std::string& aKey) const { return myVectors.find(aKey) != myVectors.end(); }

    void SetCharacterController(CharacterController* aCharController) { myCharController = aCharController; }
    CharacterController* GetCharacterController() { return myCharController; }

protected:
    std::unordered_map<std::string, bool> myBools;
    std::unordered_map<std::string, int> myInts;
    std::unordered_map<std::string, float> myFloats;
    std::unordered_map<std::string, std::string> myStrings;
    std::unordered_map<std::string, Vector3f> myVectors;
    CharacterController* myCharController = nullptr;
};