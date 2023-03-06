#pragma once
#include <any>
#include <typeindex>
#include <utility>
#include "Graph/UUID.h"
#include "ScriptGraphDataObject.h"
#include "ScriptGraphTypes.h"

class AnimationGraphSchema;
class AnimationGraphNode;
class AnimationGraphPin;


class AnimationGraphPin : public NodeGraphPin<AnimationGraphNode, AnimationGraphSchema>
{
	friend AnimationGraphSchema;
	PinIcon myIcon = PinIcon::Circle;
	bool myLabelVisible = true;

	ScriptGraphDataObject myData;

	AnimationGraphPin(const std::shared_ptr<AnimationGraphNode>& anOwner, const std::string& aLabel, PinDirection aDirection, PinIcon anIcon)
		: NodeGraphPin(anOwner, aLabel, aDirection), myIcon(anIcon)
	{  }

protected:

	void SetDataObject(ScriptGraphDataObject&& aDataObject)
	{
		myData = std::move(aDataObject);
	}

public:
	template<typename DataType>
	void InitDataBlock()
	{
		myData = AnimationGraphDataObject::Create<DataType>();
	}
	template<typename DataType>
	static AnimationGraphPin CreateDataPin(const std::shared_ptr<AnimationGraphNode>& aOwner, const std::string& aLabel, PinIcon anIcon, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AnimationGraphPin Pin(aOwner, aLabel, aDirection, anIcon, aType);
		Pin.InitDataBlock<DataType>();
		Pin.myLabelVisible = !hideLabelOnNode;
		return Pin;
	}

	static AnimationGraphPin CreatePin(const std::shared_ptr<AnimationGraphNode>& aOwner, const std::string& aLabel, PinIcon anIcon, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		AnimationGraphPin Pin(aOwner, aLabel, aDirection, anIcon);
		Pin.myLabelVisible = !hideLabelOnNode;
		return Pin;
	}

	template<typename DataType>
	bool GetData(DataType& outData) const
	{
#if _DEBUG
		const std::type_index& RequestedType = typeid(DataType);
		assert(RequestedType == myData.TypeData->GetType() && "Cannot GetData of another type than the one that is stored!");
#endif
		DataType* ptr = static_cast<DataType*>(myData.Ptr);
		if (ptr)
		{
			outData = *ptr;
			return true;
		}

		return false;
	}

	template<typename DataType>
	void SetData(DataType data)
	{
#if _DEBUG
		const std::type_index& RequestedType = typeid(DataType);
		assert(RequestedType == myData.TypeData->GetType() && "Cannot SetData of another type than the one that is stored!");
#endif
		memset(myData.Ptr, 0, myData.TypeData->GetTypeSize());
		memcpy_s(myData.Ptr, myData.TypeData->GetTypeSize(), &data, sizeof(DataType));
	}

	[[nodiscard]] void* GetPtr() const { return myData.Ptr; }

	FORCEINLINE bool IsLabelVisible() const { return myLabelVisible; }
	virtual FORCEINLINE std::type_index GetDataType() const { if (myData.TypeData) return myData.TypeData->GetType(); return typeid(std::nullptr_t); }
	virtual FORCEINLINE std::string GetSimpleDataName() const { if (myData.TypeData) return myData.TypeData->GetSimpleTypeName(); return "nullptr"; }
	virtual FORCEINLINE std::size_t GetDataSize() const { return myData.TypeData->GetTypeSize(); }
};