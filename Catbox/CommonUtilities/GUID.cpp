#include "stdafx.h"
#include "GUID.h"
#include <Windows.h>

void Catbox::GUID::GenerateNew()
{
	HRESULT result = CoCreateGuid(&myWin32Guid);
	if(FAILED(result)) print("Failed to generate GUID.");
}

Catbox::GUID::GUID(const GUID& aOther)
	: myWin32Guid(aOther.myWin32Guid)
{}

void Catbox::GUID::SetFromString(const std::string& aString)
{
	SetFromString(aString.c_str());
}

void Catbox::GUID::SetFromString(const char* aString)
{
	RPC_STATUS status = UuidFromStringA(reinterpret_cast<RPC_CSTR>(const_cast<char*>(aString)), &myWin32Guid);

	if (status != RPC_S_OK)
	{
		print("Failed to set GUID from string.");
		myWin32Guid = GUID_NULL;
	}
}

#pragma warning(push)
#pragma warning(disable : 6387)
#pragma warning(disable : 6011)
std::string Catbox::GUID::GetAsString() const
{
	RPC_CSTR buffer { nullptr };
	RPC_STATUS status = UuidToStringA(&myWin32Guid, &buffer);
	std::string guidString;
	if(status == RPC_S_OK)
	{
		guidString = reinterpret_cast<const char*>(buffer);
		RpcStringFreeA(&buffer);
	}
	else
	{
		print("Failed to convert GUID to string. RPC Status: " + status);
	}
	return guidString;
}
#pragma warning (pop)

Catbox::GUID& Catbox::GUID::operator=(const GUID& aOther)
{
	myWin32Guid = aOther.myWin32Guid;
	return *this;
}

bool Catbox::GUID::operator==(const GUID& aOther) const
{
	return InlineIsEqualGUID(myWin32Guid, aOther.myWin32Guid);
}

bool Catbox::GUID::operator!=(const GUID& aOther) const
{
	return !operator==(aOther);
}

bool Catbox::GUID::operator<(const GUID& aOther) const
{
	return memcmp(&myWin32Guid, &aOther.myWin32Guid, sizeof(_GUID)) < 0;
}

bool Catbox::GUID::operator>(const GUID& aOther) const
{
	return memcmp(&myWin32Guid, &aOther.myWin32Guid, sizeof(_GUID)) > 0;
}

bool Catbox::GUID::operator>=(const GUID& aOther) const
{
	return memcmp(&myWin32Guid, &aOther.myWin32Guid, sizeof(_GUID)) >= 0;
}

bool Catbox::GUID::operator<=(const GUID& aOther) const
{
	return memcmp(&myWin32Guid, &aOther.myWin32Guid, sizeof(_GUID)) <= 0;
}

size_t Catbox::GUID::Hash() const
{
	const uint64_t* guidAsUI64 = reinterpret_cast<const uint64_t*>(&myWin32Guid);
	return guidAsUI64[0] ^ guidAsUI64[1];
}

bool Catbox::GUID::IsNil() const
{
	return (myWin32Guid == GUID_NULL);
}

Catbox::GUID Catbox::GUID::Nil()
{
	return GUID { GUID_NULL };
}

Catbox::GUID Catbox::GUID::NewGUID()
{
	Catbox::GUID guid;
	guid.GenerateNew();
	return guid;
}

Catbox::GUID::GUID(_GUID aWin32Guid)
	: myWin32Guid(aWin32Guid)
{}

Catbox::GUID operator ""_pGUID(const char* aGUIDStr, size_t /*aStrLength*/)
{
	Catbox::GUID guid;
	guid.SetFromString(aGUIDStr);
	return guid;
}
