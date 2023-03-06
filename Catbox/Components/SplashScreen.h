#pragma once
#include "../GameObjects/Component.h"
class SplashScreen :
    public Component
{
    void Init() override;
    void Awake() override;
    void Update() override;
    void Save(rapidjson::Value& aComponentData) override;
    void Load(rapidjson::Value& aComponentData) override;
    void RenderInProperties(std::vector<Component*>& aComponentList) override;

private:
    std::string mySceneToLoad = "Undefined";
};

