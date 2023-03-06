#pragma once
#include "GameObjects\Component.h"

class UIElement : public Component
{
public:
	struct UIRenderData
	{
		Vector2f position; //8 bytes
		Vector2f scale; // 8 bytes
		Color color; //16 bytes
		int id = -1; //4 bytes
		Vector3f padding;
	} myUIRenderData;

	UIElement() = default;
	virtual ~UIElement();

	void Init() override;
	virtual void Render() {};
	void Update() override;
	void RunInEditor() override;
	void SetIsHovered(bool aIsHovered);
	virtual void OnClick() {};
	virtual void OnMouseEnter() {};
	virtual void OnMouseOver() {};
	virtual void OnMouseExit() {};
	void OnTransformChanged() override;
	inline UIRenderData& GetUIRenderData() { return myUIRenderData; }
	inline void SetAlpha(float aValue) { myUIRenderData.color.a = aValue; }
	inline float GetAlpha() { return myUIRenderData.color.a; }
	inline int GetLayer() const { return myLayer; }
	inline void SetLayer(int aLayer) { myLayer = aLayer; }


protected:
	bool myRaycastTarget = true;
	bool myIsHovered = false;
	int myLayer = 0;
	Vector2i myReferenceResolution = {1920, 1080};
};