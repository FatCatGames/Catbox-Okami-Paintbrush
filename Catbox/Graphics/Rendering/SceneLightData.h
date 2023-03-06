#pragma once

struct LightData;
class SceneLightData
{
public:
	SceneLightData() = default;
	~SceneLightData() = default;
	void AddLight(LightData* aLight);
	void RemoveLight(LightData* aLight);
	inline const int& GetPointStartIndex() const { return myPointStartIndex; }
	inline const int& GetSpotStartIndex() const { return mySpotStartIndex; }
	inline const int& GetIBLStartIndex() const { return myIBLStartIndex; }
	inline const std::vector<LightData*>& GetLights() const { return myLights; }
	void ClearLightData();

private:
	std::vector<LightData*> myLights;
	int myPointStartIndex = 0;
	int mySpotStartIndex = 0;
	int myIBLStartIndex = 0;
};