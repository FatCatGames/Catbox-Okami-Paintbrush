#pragma once
#include "UtilityFunctions.hpp"
#include "Curve.h"
#include "rapidjson/rapidjson.h"

template<typename T>
class DataProperty
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

	enum EvaluationType 
	{
		Default,
		Random,
		Curve
	};


	DataProperty();
	DataProperty(const EvaluationType& anEvaluationType);
	void SetEvaluationType(const EvaluationType& anEvaluationType);
	//inline const T& GetValue();
	//void ParseToJsonObject(rapidjson::GenericObject<false, rapidjson::Value>& aJsonObject);

private:
	friend class ParticleSystem;
	EvaluationType myEvaluationType;
	Range<T> myRange;
	//Curve myCurve;
};

template<typename T>
inline DataProperty<T>::DataProperty()
{
	myEvaluationType = EvaluationType::Default;
}

template<typename T>
inline DataProperty<T>::DataProperty(const EvaluationType& anEvaluationType)
{
	myEvaluationType = anEvaluationType;
}

template<typename T>
inline void DataProperty<T>::SetEvaluationType(const EvaluationType& anEvaluationType)
{
	myEvaluationType = anEvaluationType;
}

//template<typename T>
//inline const T& DataProperty<T>::GetValue()
//{
//	/*if (myIsRandom)
//	{
//		return Catbox::Lerp(myRange.min, myRange.max, 1.f - (myData.timePassed / myData.duration));
//	}
//	else if (myUsingCurve)
//	{
//		return curve.Evaluate(1.f - (myData.timePassed / myData.duration));
//	}
//	else
//	{
//		return myRange.min;
//	}*/
//}

template<typename T>
inline void ParseToJsonObject(rapidjson::GenericObject<false, rapidjson::Value>& /*aJsonObject*/)
{
	/*aJsonObject["min"].Set<T>(myRange.min);
	aJsonObject["max"].Set<T>(myRange.max);
	aJsonObject["curve"].Set<T>(myCurve.ParseToJsonArray());*/
}
