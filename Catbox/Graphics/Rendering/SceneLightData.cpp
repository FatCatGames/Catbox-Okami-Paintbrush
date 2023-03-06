#include "stdafx.h"
#include "SceneLightData.h"
#include "3D/Light.h"

void SceneLightData::AddLight(LightData* aLight)
{
	int insertionIndex = 0;
	for (insertionIndex; insertionIndex < myLights.size(); insertionIndex++)
	{
		if (aLight->type < myLights[insertionIndex]->type) break;
	}

	myLights.insert(myLights.begin() + insertionIndex, aLight);
	for (int i = insertionIndex + 1; i < myLights.size(); i++)
	{
		myLights[i]->index = i;
	}

	if (aLight->type < static_cast<int>(Light::LightType::Point))
	{
		++myPointStartIndex;
	}
	if (aLight->type < static_cast<int>(Light::LightType::Spot))
	{
		++mySpotStartIndex;
	}
	if (aLight->type < static_cast<int>(Light::LightType::Environment))
	{
		++myIBLStartIndex;
	}

	aLight->index = insertionIndex;
}

void SceneLightData::RemoveLight(LightData* aLight)
{
	if (aLight->type < static_cast<int>(Light::LightType::Point))
	{
		--myPointStartIndex;
	}
	if (aLight->type < static_cast<int>(Light::LightType::Spot))
	{
		--mySpotStartIndex;
	}
	if (aLight->type < static_cast<int>(Light::LightType::Environment))
	{
		--myIBLStartIndex;
	}

	myLights.erase(myLights.begin() + aLight->index);

	for (int i = aLight->index; i < myLights.size(); i++)
	{
		myLights[i]->index = i;
	}
}

void SceneLightData::ClearLightData()
{
	myLights.clear();
	myPointStartIndex = 0;
	mySpotStartIndex = 0;
	myIBLStartIndex = 0;
}
