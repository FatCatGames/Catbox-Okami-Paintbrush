#pragma once
#include "../External/rapidjson/rapidjson.h"
#include "../External/rapidjson/document.h"
#include "DataTypes.h"

class RapidJsonWrapper
{
public:
	RapidJsonWrapper();
	~RapidJsonWrapper() = default;
	static RapidJsonWrapper* GetInstance() { return myInstance; }
	rapidjson::Value* GetTarget() { return myTarget; }
	rapidjson::Document& CreateDocument();
	rapidjson::Document& GetDocument() { return myDocument; }
	rapidjson::Value CreateObject();
	rapidjson::Value CreateArray();
	void ClearDocument();
	void SetTarget(rapidjson::Value& anArray);
	void ClearTarget();
	void FreeMemory(void* aPtr) { myAlloc.Free(aPtr); }
	template<DataType Type, typename T>
	void SaveValue(const char* aVariableName, T& aVariable);
	void SaveString(const char* aVariableName, const char* aValue);
	void SaveObject(const char* aVariableName, rapidjson::Value& anObject);
	void SaveDocument(const char* aPath);
	void SaveDocumentCompressed(const char* aPath);
	std::string JsonToString(rapidjson::Value& anObject);
	rapidjson::Document& StringToJson(std::string& aString);

	bool DocumentExists(const char* aPath);
	rapidjson::Document& LoadDocument(const char* aPath);
	inline rapidjson::Value& LoadValue(const char* aVariableName);

private:
	static RapidJsonWrapper* myInstance;
	rapidjson::Document myDocument;
	rapidjson::Document myDocument2;
	rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> myAlloc;
	rapidjson::Value myTempValue;
	rapidjson::Value* myTarget;
	char readBuffer[16384]{};
	FILE* myFile;
};

template<DataType Type, typename T>
inline void RapidJsonWrapper::SaveValue(const char* aVariableName, T& aVariable)
{
	assert(myTarget && "Tried to call SaveValue with no target set!");
	//myTarget->SetObject();
	auto name = rapidjson::StringRef(aVariableName);
	switch (Type)
	{
	case DataType::Bool: 
	{
		bool value = static_cast<bool>(aVariable);
		myTempValue.SetBool(value);
		myTarget->AddMember(name, myTempValue, myAlloc);
		break;
	}
	case DataType::Float: 
	{
		float value = static_cast<float>(aVariable);
		myTempValue.SetFloat(value);
		myTarget->AddMember(name, myTempValue, myAlloc);
		break;
	}
	case DataType::Int: 
	{
		int value = static_cast<int>(aVariable);
		myTempValue.SetInt(value);
		myTarget->AddMember(name, myTempValue, myAlloc);
		break;
	}
	case DataType::String: 
	{
		assert(true && "Template function SaveValue does not work with type String. Use SaveString() instead!");
		break;
	}
	}
}

inline rapidjson::Value& RapidJsonWrapper::LoadValue(const char* aVariableName)
{
	assert(myTarget && "Tried to call LoadValue with no target set!");
	return (*myTarget)[aVariableName];
}
