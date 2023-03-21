#pragma once
#include "CommonUtilities/UtilityFunctions.hpp"
#include "CommonUtilities/Curve.h"
#include "../External/rapidjson/rapidjson.h"
#include "../External/rapidjson/document.h"


enum class EvaluationType
{
	Default,
	Random,
	Curve,
	ValueOverTime
};


template<typename T>
class ParticleProperty
{
public:

	template<typename T>
	struct Range
	{
		Range() = default;
		Range(T aMin, T aMax)
		{
			min = aMin;
			max = aMax;
		}
		T min;
		T max;
	};


	ParticleProperty();
	~ParticleProperty() = default;
	void SetEvaluationType(const EvaluationType& anEvaluationType);
	inline void SetDefaultValue(const T& aValue) { myDefaultValue = aValue; }
	void SetName(const std::string& aName);
	const EvaluationType& GetEvaluationType();
	inline const T GetValue(const float& aValueToEvaluate);
	void LoadFromJson(rapidjson::Value& aJsonObject);
	void ParseToJsonObject(rapidjson::Value& aEmissionData, rapidjson::Document& aOutput);

private:
	friend class ParticleEmitterSettings;
	EvaluationType myEvaluationType;
	T myDefaultValue;
	Range<T> myRange;
	Curve myCurve;
	std::string myName;
};


template<typename T>
inline ParticleProperty<T>::ParticleProperty()
{
	myEvaluationType = EvaluationType::Default;
	myDefaultValue = 1;
	myRange.min = 0;
	myRange.max = 1;
}

template<typename T>
inline void ParticleProperty<T>::SetEvaluationType(const EvaluationType& anEvaluationType)
{
	myEvaluationType = anEvaluationType;
}

template<typename T>
inline const EvaluationType& ParticleProperty<T>::GetEvaluationType()
{
	return myEvaluationType;
}


template<typename T>
inline const T ParticleProperty<T>::GetValue(const float& aValueToEvaluate)
{
	switch (myEvaluationType)
	{
	case EvaluationType::Default: return myDefaultValue;
	case EvaluationType::Random: return Catbox::GetRandom(myRange.min, myRange.max);
	case EvaluationType::ValueOverTime: return Catbox::Lerp(myRange.min, myRange.max, aValueToEvaluate);
	case EvaluationType::Curve: return static_cast<T>(myCurve.Evaluate(aValueToEvaluate));
	}
	return 0;
}

template<typename T>
inline void ParticleProperty<T>::LoadFromJson(rapidjson::Value& aJsonObject)
{
	rapidjson::Value& property = aJsonObject[myName.c_str()].GetObj();
	std::string evaluationTypeStr = property["evaluationType"].GetString();
	if (evaluationTypeStr == "default") 
	{
		myEvaluationType = EvaluationType::Default;
		myDefaultValue = property["default"].Get<T>();
	}
	else if (evaluationTypeStr == "curve") 
	{
		myEvaluationType = EvaluationType::Curve;
	}
	else 
	{
		if (evaluationTypeStr == "random") 
		{
			myEvaluationType = EvaluationType::Random;
		}
		else if (evaluationTypeStr == "valueOverTime") 
		{
			myEvaluationType = EvaluationType::ValueOverTime;
		}

		myRange.min = property["min"].Get<T>();
		myRange.max = property["max"].Get<T>();
	}
}

template<typename T>
inline void ParticleProperty<T>::ParseToJsonObject(rapidjson::Value& aEmissionData, rapidjson::Document& aOutput)
{
	rapidjson::Value value;
	value.SetObject();

	rapidjson::GenericStringRef<char> name = rapidjson::GenericStringRef<char>(myName.c_str());

	std::string evaluationTypeStr = "";
	if (myEvaluationType == EvaluationType::Default)
	{
		evaluationTypeStr = "default";
		value.AddMember("default", myDefaultValue, aOutput.GetAllocator());

	}
	else if (myEvaluationType == EvaluationType::Curve) 
	{
		evaluationTypeStr = "curve";
		//data["curve"].Set<T>(myCurve.ParseToJsonArray());
	}
	else 
	{
		if(myEvaluationType == EvaluationType::Random) evaluationTypeStr = "random";
		else if(myEvaluationType == EvaluationType::ValueOverTime) evaluationTypeStr = "valueOverTime";

		value.AddMember("min", myRange.min, aOutput.GetAllocator());
		value.AddMember("max", myRange.max, aOutput.GetAllocator());
	}

	rapidjson::Value evaluationType;
	evaluationType.SetString(evaluationTypeStr.c_str(), aOutput.GetAllocator());
	value.AddMember("evaluationType", evaluationType, aOutput.GetAllocator());
	aEmissionData.AddMember(name, value, aOutput.GetAllocator());
}

template<typename T>
inline void ParticleProperty<T>::SetName(const std::string& aName)
{
	myName = aName;
}
