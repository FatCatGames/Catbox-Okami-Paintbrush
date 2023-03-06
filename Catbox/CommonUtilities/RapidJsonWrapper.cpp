#include "stdafx.h"
#include "RapidJsonWrapper.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>

RapidJsonWrapper* RapidJsonWrapper::myInstance;

RapidJsonWrapper::RapidJsonWrapper()
{
	myInstance = this;
	myDocument = rapidjson::Document();
	myAlloc = myDocument.GetAllocator();
}

rapidjson::Document& RapidJsonWrapper::CreateDocument()
{
	myAlloc.Clear();
	myDocument.RemoveAllMembers();
	myDocument.SetObject();
	myTarget = &myDocument;
	return myDocument;
}

rapidjson::Value RapidJsonWrapper::CreateObject()
{
	rapidjson::Value object(rapidjson::kObjectType);
	object.SetObject();
	return object;
}


rapidjson::Value RapidJsonWrapper::CreateArray()
{
	rapidjson::Value array(rapidjson::kArrayType);
	return array;
}

void RapidJsonWrapper::ClearDocument()
{
	myAlloc.Clear();
	myTarget = &myDocument;
}

void RapidJsonWrapper::SetTarget(rapidjson::Value& anObject)
{
	myTarget = &anObject;
}

void RapidJsonWrapper::ClearTarget()
{

	myTarget = &myDocument;
}


void RapidJsonWrapper::SaveString(const char* aVariableName, const char* aValue)
{
	if (!Catbox::CheckText(aValue, std::strlen(aValue))) 
	{
		std::string aMessage = "Could not save! Text seems to not be null terminated! Text is : ";
		aMessage.append(aValue);
		printerror(aMessage);
	}
	myTempValue.SetString(rapidjson::StringRef(aValue));
	myTarget->AddMember(rapidjson::StringRef(aVariableName), myTempValue, myAlloc);
}

void RapidJsonWrapper::SaveObject(const char* aVariableName, rapidjson::Value& anObject)
{
	myTarget->AddMember(rapidjson::StringRef(aVariableName), anObject, myAlloc);
}

void RapidJsonWrapper::SaveDocument(const char* aPath)
{
	DWORD attributes = GetFileAttributesA(aPath);
	if (attributes & FILE_ATTRIBUTE_READONLY) 
	{
		SetFileAttributesA(aPath, attributes & ~FILE_ATTRIBUTE_READONLY);
	}
	std::ofstream ofs(aPath);
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	myDocument.Accept(writer);
	myTarget = nullptr;
	myDocument.RemoveAllMembers();
}

void RapidJsonWrapper::SaveDocumentCompressed(const char* aPath)
{
	DWORD attributes = GetFileAttributesA(aPath);
	if (attributes & FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributesA(aPath, attributes & ~FILE_ATTRIBUTE_READONLY);
	}
	std::ofstream ofs(aPath);
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	myDocument.Accept(writer);
	myTarget = nullptr;
	myDocument.RemoveAllMembers();
}

bool RapidJsonWrapper::DocumentExists(const char* aPath)
{
	bool error = fopen_s(&myFile, aPath, "rb");
	if (error) return false;
	fclose(myFile);
	return true;
}

rapidjson::Document& RapidJsonWrapper::LoadDocument(const char* aPath)
{
	bool error = fopen_s(&myFile, aPath, "rb");
	assert(!error && "Could not open file path");
	rapidjson::FileReadStream is(myFile, readBuffer, sizeof(readBuffer));
	myDocument.ParseStream(is);
	fclose(myFile);
	myTarget = &myDocument;
	return myDocument;
}

std::string RapidJsonWrapper::JsonToString(rapidjson::Value& anObject)
{
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	anObject.Accept(writer);

	return _strdup(buffer.GetString());
}

rapidjson::Document& RapidJsonWrapper::StringToJson(std::string& aString)
{
	myDocument2.Parse<0>(aString.c_str());
	return myDocument2;
}
