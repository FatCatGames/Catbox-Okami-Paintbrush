#include "EditorWindow.h"

class Scene;
class SceneHierarchyWindow : public EditorWindow
{
public:
	void SetScene(Scene* aScene);
	inline std::vector<GameObject*>& GetSelectedObjects() { return mySelectedObjects; }
	void ClearSelectedObjects();
	void AddSelectedObject(GameObject* anObject);
	void RemoveSelectedObject(GameObject* anObject);
	void FocusOnObject(GameObject* anObject);
	void Reset();


private:
	void RenderObject(GameObject* anObject);
	void ItemClickedCheck(GameObject* anObject);
	bool ScrollCheck(GameObject* anObject);
	void Render() override;
	void SearchChildren(std::vector<GameObject*>& aGameObjectRefrence, GameObject* aObject);
	Scene* myScene;
	bool myPopupRendered = false;
	bool myRenderParentingPopup = false;
	std::vector<GameObject*> mySelectedObjects;
	std::vector<GameObject*> myDraggedObjects;
	std::vector<GameObject*> myRenderedObjects;
	std::vector<GameObject*> myShouldRenderObjects;
	GameObject* mySelectedParent;

	int mySelectedObjectIndex = -1;
	int myObjectIDToScrollTo = -1;
	int myDragIndex = 0;
	std::string mySearchString = "";
};