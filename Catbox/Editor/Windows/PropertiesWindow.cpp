#include "stdafx.h"
#include "PropertiesWindow.h"
#include "EditorWindow.h"

//this shouldnt be here
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include "CommonUtilities/Color.h"

void PropertiesWindow::SetSelected(Asset* anAsset)
{
	for (size_t i = 0; i < mySelectedAssets.size(); i++)
	{
		mySelectedAssets[i]->SetSelected(0, false);
	}

	mySelectedAssets.clear();
	if (anAsset)
	{
		mySelectedAssets.push_back(anAsset);
		anAsset->SetSelected(0, true);
	}
}

void PropertiesWindow::AddSelected(Asset* anAsset)
{
	mySelectedAssets.push_back(anAsset);
	anAsset->SetSelected(0, true);
}

void PropertiesWindow::DeselectAll()
{
	for (size_t i = 0; i < mySelectedAssets.size(); i++)
	{
		mySelectedAssets[i]->SetSelected(0, false);
	}
	mySelectedAssets.clear();
}

void PropertiesWindow::Deselect(Asset* anAsset)
{
	for (size_t i = 0; i < mySelectedAssets.size(); i++)
	{
		if (mySelectedAssets[i] == anAsset) 
		{
			anAsset->SetSelected(0, false);
			mySelectedAssets.erase(mySelectedAssets.begin() + i);
			return;
		}
	}
}

void PropertiesWindow::Render()
{
	if (!BeginWindow("Properties", false)) return;

	if (mySelectedAssets.size() > 0)
	{
		if (mySelectedAssets.size() == 1)
		{
			mySelectedAssets[0]->RenderInProperties();
		}
		else 
		{
			mySelectedAssets[0]->RenderInProperties(mySelectedAssets);
		}
	}
	else
	{
		ImGui::Text("Select an object in the Hierarchy window");
	}
	ImGui::End();
}
