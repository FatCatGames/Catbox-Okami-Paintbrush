#include "stdafx.h"
#include "AssetBrowserWindow.h"
#include "EditorWindow.h"
#include "Components/3D/ModelInstance.h"
#include "Assets/Model.h"
#include "Components/Animator.h"
#include "CommonUtilities/InputHandler.h"
#include "ComponentTools/SceneManager.h"
#include "Components/ParticleSystem.h"
#include "Editor/Windows/PropertiesWindow.h"
#include "Assets/Asset.h"
#include "Assets/Blueprint.h"
#include "Assets/Material.h"
#include "ImGui/imgui_stdlib.h"
#include <string>
#include "Assets\AnimationController.h"
static const std::filesystem::path AssetPath = "Assets\\";


AssetBrowserWindow::AssetBrowserWindow()
{
	AssetLoader::LoadAllAssets();
	myCurrentPath.push_back(&AssetLoader::myAssetFolder);
	myCurrentDirectory = &AssetLoader::myAssetFolder;
	myStartertDirectory = &AssetLoader::myAssetFolder;
	myDirectoryTex = AssetLoader::LoadTexture("Resources/Icons/DirectoryIcon.dds");
	myFileTex = AssetLoader::LoadTexture("Resources/Icons/FileIcon.dds");
	//myAssetFolder = AssetRegistry::GetInstance()->GetAssetFolder();

	//I'm an official engine(C) programmer(TM) now :)
	myFileIconExtensions["_mesh.fbx"] = AssetLoader::LoadTexture("Resources/Icons/MeshIcon.dds");
	myFileIconExtensions["_anim.fbx"] = AssetLoader::LoadTexture("Resources/Icons/AnimationIcon.dds");
	myFileIconExtensions["_sk.fbx"] = AssetLoader::LoadTexture("Resources/Icons/SkeletonIcon.dds");
	myFileIconExtensions["_emit.pe"] = AssetLoader::LoadTexture("Resources/Icons/ParticleEmitterIcon.dds");
	myFileIconExtensions["_mat.mat"] = AssetLoader::LoadTexture("Resources/Icons/MaterialIcon.dds");
	myFileIconExtensions["_lvl.scene"] = AssetLoader::LoadTexture("Resources/Icons/SceneIcon.dds");
	myFileIconExtensions[".prefab"] = AssetLoader::LoadTexture("Resources/Icons/PrefabIcon.dds");
	myFileIconExtensions[".profile"] = AssetLoader::LoadTexture("Resources/Icons/ProfileIcon.dds");
	myFileIconExtensions[".obj"] = AssetLoader::LoadTexture("Resources/Icons/ObjIcon.dds");
	myFileIconExtensions[".nav"] = AssetLoader::LoadTexture("Resources/Icons/NavIcon.dds");
	myFileIconExtensions[".bp"] = AssetLoader::LoadTexture("Resources/Icons/BlueprintIcon.dds");
	myFileIconExtensions[".hlsl"] = AssetLoader::LoadTexture("Resources/Icons/HLSLIcon.dds");
	myFileIconExtensions["OVER"] = AssetLoader::LoadTexture("Resources/Icons/NAMNSTANDARD.dds");
}

void AssetBrowserWindow::AddAssetToCurrentFolder(std::shared_ptr<Asset> anAsset)
{
	myCurrentDirectory->assets.push_back(anAsset);
}



void AssetBrowserWindow::Render()
{
	if (!ImGui::Begin("Asset Browser", NULL, ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		ImGui::End();
		return;
	}
	RenderInFrontCheck();

	static float zoomLevel = 1.f;
	float iconSize = 90.f * zoomLevel;
	float padding = 10.f;


	int columnCount = static_cast<int>(ImGui::GetContentRegionAvail().x / (iconSize + padding));
	if (columnCount < 1) columnCount = 1;
	ImGui::Text("Zoom");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	Catbox::SliderFloat("##ZoomSlider", &zoomLevel, 0.5f, 1.5f);
	ImGui::SameLine();

	ImGui::PushItemWidth(200);
	ImGui::InputText("Search Field", &mySearchTerm);
	if (!mySearchTerm.empty())
	{
		myCurrentSearchAssets = Search(mySearchTerm);
	}
	else
	{
		myCurrentSearchAssets.clear();
	}
	bool itemHovered = false;

	ImGui::NewLine();
	for (size_t i = 0; i < myCurrentPath.size(); i++)
	{
		ImGui::SameLine();
		ImGui::Text(myCurrentPath[i]->name.c_str());
		if (ImGui::IsItemHovered(0) && ImGui::IsItemClicked(0))
		{
			while (!myRedoQueue.empty())
			{
				myRedoQueue.pop();
			}
			myUndoQueue.push_back(myCurrentPath);
			if (myUndoQueue.size() > 10)
			{
				myUndoQueue.pop_front();
			}
			myCurrentPath.erase(myCurrentPath.begin() + i + 1, myCurrentPath.end());
			myCurrentDirectory = myCurrentPath.back();
			break;
		}

		if (i != myCurrentPath.size() - 1)
		{
			ImGui::SameLine();
			ImGui::Text("/");
		}
	}

	if (Input::GetKeyPress(KeyCode::MOUSEXBUTTON2) && !myUndoQueue.empty())
	{
		myRedoQueue.push(myCurrentPath);
		myCurrentPath = myUndoQueue.back();
		myUndoQueue.pop_back();
		myCurrentDirectory = myCurrentPath.back();
	}
	else if (Input::GetKeyPress(KeyCode::MOUSEXBUTTON1))
	{
		if (!myRedoQueue.empty())
		{
			myUndoQueue.push_back(myCurrentPath);
			if (myUndoQueue.size() > 10)
			{
				myUndoQueue.pop_front();
			}
			myCurrentPath = myRedoQueue.top();
			myCurrentDirectory = myCurrentPath.back();
			myRedoQueue.pop();
		}
	}

	if (ImGui::BeginTable("AssetsTable", columnCount))
	{
		if (mySearchTerm.empty())
		{
			for (auto& directory : myCurrentDirectory->directories)
			{
				ImGui::TableNextColumn();

				ImGui::ImageButton(static_cast<ImTextureID>(myDirectoryTex->GetShaderResourceView().Get()), { iconSize, iconSize });


				if (ImGui::IsItemHovered())
				{
					itemHovered = true;
					if (ImGui::IsMouseDoubleClicked(0))
					{
						while (!myRedoQueue.empty())
						{
							myRedoQueue.pop();
						}
						myUndoQueue.push_back(myCurrentPath);
						if (myUndoQueue.size() > 10)
						{
							myUndoQueue.pop_front();
						}
						myCurrentDirectory = &directory;
						myCurrentPath.push_back(myCurrentDirectory);
					}
				}
				std::string temp;
				temp.resize(32);
				std::string aString = directory.name;
				for (size_t i = 0; i < directory.name.size(); i++)
				{
					temp[i] = directory.name[i];
				}
				ImGui::Text(&temp[0]);
			}
		}

		auto& assets = mySearchTerm.empty() ? myCurrentDirectory->assets : myCurrentSearchAssets;
		int counter = 0;
		for (auto& asset : assets)
		{
			if (counter < 100) 
			{
				counter++;
				if (!asset) continue;
				ImGui::TableNextColumn();
				//ImGui::ImageButton(static_cast<ImTextureID>(myFileTex->GetShaderResourceView().Get()), { iconSize, iconSize });

				std::string name = asset->GetPathStr();

				if (asset->GetPath().extension().generic_string() == ".dds")
				{
					ImGui::ImageButton(static_cast<ImTextureID>(AssetRegistry::GetInstance()->GetAssetByPath<Texture>(asset->GetPath().generic_string())->GetShaderResourceView().Get()), { iconSize, iconSize });
				}
				else if (myFileIconExtensions.count(asset->GetPath().extension().generic_string()))
				{
					ImGui::ImageButton(static_cast<ImTextureID>(myFileIconExtensions[asset->GetPath().extension().generic_string()]->GetShaderResourceView().Get()), { iconSize, iconSize });
				}
				else if(name.find_last_of('_') != std::string::npos)
				{
					name = name.substr(name.find_last_of('_'));
					//printerror(name); why is this here

					if (myFileIconExtensions.count(name))
					{
						ImGui::ImageButton(static_cast<ImTextureID>(myFileIconExtensions[name]->GetShaderResourceView().Get()), { iconSize, iconSize });
					}
					else
					{
						ImGui::ImageButton(static_cast<ImTextureID>(myFileIconExtensions["OVER"]->GetShaderResourceView().Get()), { iconSize, iconSize });
					}
				}
				else
				{
					ImGui::ImageButton(static_cast<ImTextureID>(myFileIconExtensions["OVER"]->GetShaderResourceView().Get()), {iconSize, iconSize});
				}

				if (ImGui::IsItemHovered())
				{
					itemHovered = true;

					if (ImGui::IsMouseDoubleClicked(0))
					{
						asset->DoubleClickAsset();
					}

					if (myHoveredAsset == asset)
					{
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							print("start drag " + asset->myName);
							myDragDropPath = asset->myPath;
							std::string tag = "DRAG_ASSET" + myDragDropPath.extension().string();
							ImGui::SetDragDropPayload(tag.c_str(), &myDragDropPath, sizeof(std::filesystem::path));
							ImGui::EndDragDropSource();
						}
						else if (ImGui::IsMouseReleased(0))
						{
							std::vector<PropertiesWindow*> propertiesWindows = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<PropertiesWindow>(WindowType::Properties);
							for (size_t i = 0; i < propertiesWindows.size(); i++)
							{
								if (!propertiesWindows[i]->IsLocked())
								{
									propertiesWindows[i]->SetSelected(asset.get());
									break;
								}
							}

							myHoveredAsset = nullptr;
						}
					}
					else if (ImGui::IsMouseClicked(0))
					{
						myHoveredAsset = asset;
					}
				}
				//else myHoveringAssetSinceClick = false;
				std::string tempString;
				std::string aString = asset->GetName();
				tempString.resize(aString.size());
				assert(tempString.size() == asset->GetName().size());
				for (size_t i = 0; i < aString.size(); i++)
				{
					tempString[i] = aString[i];
				}
				ImGui::Text(tempString.c_str());
			}
		}
		ImGui::EndTable();
	}



	if (!itemHovered && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("assetbrowser_menu");
	}
	if (ImGui::BeginPopup("assetbrowser_menu"))
	{


		CreateAssetDropdown<Blueprint>("Blueprint", ".bp");
		CreateAssetDropdown<AnimationController>("Animation Controller", ".ac");
		CreateAssetDropdown<Material>("Material", ".mat");
		CreateAssetDropdown<Scene>("Scene", ".scene");
		CreateAssetDropdown<PixelShader>("PixelShader", ".hlsl");
		CreateAssetDropdown<VertexShader>("VertexShader", ".hlsl");
		CreateAssetDropdown<GeometryShader>("GeometryShader", ".hlsl");

		if (ImGui::MenuItem("Refresh"))
		{
			std::vector<string> myPreviousPath;
			for (int i = 0; i < myCurrentPath.size(); i++)
			{
				myPreviousPath.push_back(myCurrentPath[i]->name);
			}

			AssetLoader::LoadAllAssets();

			std::vector<AssetFolder*> refreshedPath;
			refreshedPath.push_back(&AssetLoader::myAssetFolder);
			AssetFolder* currentDir = &AssetLoader::myAssetFolder;
			for (int i = 1; i < myPreviousPath.size(); i++)
			{
				bool success = false;
				for (auto& directory : currentDir->directories)
				{
					if (directory.name == myPreviousPath[i])
					{
						success = true;
						refreshedPath.push_back(&directory);
						currentDir = &directory;
						break;
					}
				}

				if (!success) break;
			}
			if (refreshedPath.size() == myPreviousPath.size())
			{
				myCurrentPath = refreshedPath;
			}
			else
			{
				myCurrentPath.clear();
				myCurrentPath.push_back(&AssetLoader::myAssetFolder);
			}
			myCurrentDirectory = myCurrentPath.back();
		}
		ImGui::EndPopup();
	}
	ImGui::End();

}

std::vector<std::shared_ptr<Asset>> AssetBrowserWindow::Search(std::string& aTerm)
{
	std::vector<std::shared_ptr<Asset>> myAssets = AssetRegistry::GetInstance()->GetAllAssets();
	std::vector<std::shared_ptr<Asset>> myReturnAssets;
	for (int i = 0; i < aTerm.size(); i++)
	{
		aTerm[i] = static_cast<char>(std::tolower(aTerm[i]));
	}
	for (auto& asset : myAssets)
	{
		if (asset != nullptr)
		{
			std::string tempAsset;
			for (auto& aChar : asset->GetName())
			{
				tempAsset.push_back(static_cast<char>(std::tolower(aChar)));
			}
			if (tempAsset.find(aTerm) != std::string::npos || aTerm.empty())
			{
				myReturnAssets.push_back(asset);
			}
		}
	}
	return myReturnAssets;
}