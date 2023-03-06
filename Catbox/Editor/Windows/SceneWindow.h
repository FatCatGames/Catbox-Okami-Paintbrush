#pragma once
#include "EditorWindow.h"

struct ImVec2;
class SceneWindow : public EditorWindow
{
public:
	SceneWindow();
	inline bool IsHovered() const { return myIsHovered; }
	inline bool WasResizedLastFrame() const { return myResizedLastFrame; }
	Vector2f GetPosition() const { return { myPos.x, myPos.y }; }
	Vector2f GetSize() const { return { mySize.x, mySize.y }; }
	inline Texture& GetTexture() { return myTexture; }

private:
	void Render() override;
	bool DrawPlayButton(std::shared_ptr<Texture>& aTexture, float& anIconSize, float anOffset, float aTint);
	void SearchChildren(std::vector<GameObject*>& aGameObjectRefrence, GameObject* aObject);
	std::shared_ptr<SceneManager> mySceneManager;
	bool myIsHovered = false;
	bool myResizedLastFrame = true;
	D3D11_VIEWPORT myViewPort;
	ImVec2 mySize;
	ImVec2 myPos;
	int myFrames = 0;
	Texture myTexture;
	std::shared_ptr<Texture> myPlayButton;
	std::shared_ptr<Texture> myPauseButton;
	std::shared_ptr<Texture> myResumeButton;
	std::shared_ptr<Texture> myStopButton;
	bool myCanEscExit = false;
};