#pragma once

enum class WindowType
{
	Properties,
	SceneHierarchy,
	CatboxSettings,
	AssetBrowser,
	Scene,
	PostProcessingEditor,
	DebugRenderer,
	Console,
	Physics,
	Navigation,
	GeneralTools,
	Shader,
	Performace,
	VisualScripting,
	HotKey,
	AnimationControllerEditor,
	ChatWindow
};

typedef int ImGuiWindowFlags;
class EditorWindow
{
public:
	EditorWindow() = default;
	~EditorWindow() = default;
	inline WindowType GetType() const { return myType; }
	void RenderInFrontCheck();
	bool IsOpen() { return myIsOpen; }
	inline bool IsFocused() const { return myIsFocused; }


protected:
	friend class EditorWindowHandler;
	virtual void Render() = 0;
	bool BeginWindow(const char* aWindowName, bool aCanBeClosed, ImGuiWindowFlags flags = 0);
	void EndWindow();
	WindowType myType;
	bool myShouldOpen = true;
	bool myIsOpen = false;
	bool myIsFocused = false;
};