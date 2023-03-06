#pragma once
#include <guiddef.h>
#include <cguid.h>
#include <string>

namespace Catbox
{
	class GUID
	{
		public:
			GUID() = default;
			GUID(const GUID& aOther);
			~GUID() = default;

			void GenerateNew();

			void SetFromString(const std::string& aString);
			void SetFromString(const char* aString);
			std::string GetAsString() const;

			GUID& operator=(const GUID& aOther);

			bool operator==(const GUID& aOther) const;
			bool operator!=(const GUID& aOther) const;
			bool operator<(const GUID& aOther) const;
			bool operator>(const GUID& aOther) const;
			bool operator>=(const GUID& aOther) const;
			bool operator<=(const GUID& aOther) const;

			size_t Hash() const;

			bool IsNil() const;

			static GUID Nil();
			static GUID NewGUID();
		private:
			_GUID myWin32Guid { GUID_NULL };

			explicit GUID(_GUID aWin32Guid);
	};
}

Catbox::GUID operator""_pGUID(const char* aGUIDStr, size_t aStrLength);

template<>
struct std::hash<Catbox::GUID>
{
	std::size_t operator()(const Catbox::GUID& aGUID) const noexcept
	{
		return aGUID.Hash();
	}
};
