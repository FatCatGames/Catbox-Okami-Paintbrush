#include "stdafx.h"
#include "ParticleEmitterSettings.h"
#include "Assets/Texture.h"
#include "Graphics/Rendering/DX11/DX11.h"
#include "Assets/Shader.h"
#include "Assets\Material.h"

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include "Shaders\FlipbookShader.h"

static ColorPoint* draggingMark = nullptr;
static ColorPoint* selectedMark = nullptr;

ParticleEmitterSettings::ParticleEmitterSettings()
{
	myName = "New emitter";
	mySizeOverLifetime.AddCoord({ 0,1 });
	mySizeOverLifetime.AddCoord({ 1,0 });

	mySpeedOverLifetime.AddCoord({ 0,1 });
	mySpeedOverLifetime.AddCoord({ 1,0 });

	myRotationOverLifetime.AddCoord({ 0,0 });
	myRotationOverLifetime.AddCoord({ 1,1 });

	myVelocityOverLifetimeX.AddCoord({ 0,0 });
	myVelocityOverLifetimeX.AddCoord({ 1,0 });
	myVelocityOverLifetimeY.AddCoord({ 0,0 });
	myVelocityOverLifetimeY.AddCoord({ 1,0 });
	myVelocityOverLifetimeZ.AddCoord({ 0,0 });
	myVelocityOverLifetimeZ.AddCoord({ 1,0 });

	mySize.SetName("Size");
	mySpeed.SetName("Speed");
	myLifetime.SetName("Lifetime");
	myRotation.SetName("Rotation");
	myTimeBetweenEmissions.SetName("Time between emissions");
	myParticlesPerEmission.SetName("Particles per emission");

	myParticlesPerEmission.SetDefaultValue(10);

	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	myStride = sizeof(ParticleBufferData);
	myOffset = 0;
	myVertexBufferDesc.ByteWidth = static_cast<UINT>(myMaxParticles * static_cast<UINT>(sizeof(ParticleBufferData)));
	myVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	myVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	myVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	myVertexBufferDesc.MiscFlags = 0;
	myVertexBufferDesc.StructureByteStride = 0;

	myTexture = AssetRegistry::GetInstance()->GetAsset<Texture>("ParticleStar");
	myVertexShader = AssetRegistry::GetInstance()->GetAsset<VertexShader>("ParticleVS");
	myPixelShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>("ParticlePS");
	myGeometryShader = AssetRegistry::GetInstance()->GetAsset<GeometryShader>("ParticleGS");
	myMaterial = AssetRegistry::GetInstance()->GetAsset<Material>("ParticleDefaultMat");
	myDefaultMaterial = myMaterial;

	myFlipbookShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>("FlipbookPS")->MakeInstance();
	myFlipbookShader->pixelShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>("ParticleFlipbookPS")->pixelShader;
	myFlipbookData = &dynamic_cast<FlipbookShader*>(myFlipbookShader.get())->myFlipbookData;

	myAtlasWalkShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>("ParticleAtlasWalkPS")->MakeInstance();
	myAtlasWalkShader->pixelShader = AssetRegistry::GetInstance()->GetAsset<PixelShader>("ParticleAtlasWalkPS")->pixelShader;
	myAtlasWalkData = &dynamic_cast<ParticleAtlasWalkShader*>(myAtlasWalkShader.get())->myParticleAtlasWalkData;
}


void ParticleEmitterSettings::RenderInProperties()
{
	bool pushColor = myHasUnsavedChanges;
	if (pushColor)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.3f, 1 });
		ImGui::Text("Unsaved changes!");
	}

	if (ImGui::Button("Save", { 70, 30 }))
	{
		if (myPath.empty())
		{
			myPath = Catbox::SaveFile("Particle Emitter (*.pe)\0*.pe\0");
			if (!myPath.empty())
			{
				SaveAsset(myPath.string().c_str());
				ExtractNameFromPath();
				myHasUnsavedChanges = false;
			}
		}
		else
		{
			SaveAsset(myPath.string().c_str());
			myHasUnsavedChanges = false;
		}
	}

	if (pushColor)
	{
		ImGui::PopStyleColor();
	}


	ImGui::SameLine();
	if (ImGui::Button("Save As...", { 80, 30 }))
	{
		myPath = Catbox::SaveFile("Particle Emitter (*.pe)\0*.pe\0");
		if (!myPath.empty())
		{
			SaveAsset(myPath.string().c_str());
			ExtractNameFromPath();
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	RenderEmissionSettings();
	RenderShapeSettings();
	RenderOverLifetimeSettings();
	RenderGraphicsSettings();
}


void ParticleEmitterSettings::SaveAsset(const char* /*aPath*/)
{
	myVersion = 7;
	rapidjson::Document output;
	output.SetObject();
	auto& alloc = output.GetAllocator();
	rapidjson::Value value;

	value.SetInt(myVersion);
	output.AddMember("Version", value, alloc);

#pragma region Emission
	rapidjson::Value emissionData;
	emissionData.SetObject();
	output.AddMember("Emission Data", emissionData, alloc);
	rapidjson::GenericObject<false, rapidjson::Value> ed = output["Emission Data"].GetObj();

	value.SetBool(myUseWorldSpace);
	ed.AddMember("WorldSpace", value, alloc);

	value.SetBool(myIsLooping);
	ed.AddMember("Looping", value, alloc);

	value.SetInt(myMaxParticles);
	ed.AddMember("Max particles", value, alloc);

	value.SetFloat(myDuration);
	ed.AddMember("Duration", value, alloc);

	value.SetFloat(myStartDelay);
	ed.AddMember("Delay", value, alloc);

	mySize.ParseToJsonObject(ed, output);
	mySpeed.ParseToJsonObject(ed, output);
	myLifetime.ParseToJsonObject(ed, output);
	myRotation.ParseToJsonObject(ed, output);
	myTimeBetweenEmissions.ParseToJsonObject(ed, output);
	myParticlesPerEmission.ParseToJsonObject(ed, output);

	myStartColor.ParseToJsonObject(ed, output, "Start color");

	value.SetBool(myUseGravity);
	ed.AddMember("Use gravity", value, alloc);
	value.SetFloat(myGravityModifier);
	ed.AddMember("Gravity modifier", value, alloc);

#pragma endregion

#pragma region Shape

	rapidjson::Value shapeData;
	shapeData.SetObject();
	output.AddMember("Shape Data", shapeData, alloc);
	rapidjson::GenericObject<false, rapidjson::Value> sd = output["Shape Data"].GetObj();
	value.SetInt(static_cast<int>(myShape));
	sd.AddMember("Shape", value, alloc);

#pragma endregion

#pragma region Over lifetime
	rapidjson::Value overLifetime;
	overLifetime.SetObject();
	output.AddMember("Over lifetime", overLifetime, alloc);
	rapidjson::GenericObject<false, rapidjson::Value> ol = output["Over lifetime"].GetObj();

	rapidjson::Value sizeOverLifetime;
	sizeOverLifetime.SetObject();
	value.SetBool(mySizeOverLifetimeEnabled);
	mySizeOverLifetime.ParseToJsonObject(sizeOverLifetime, output, "Curve");
	sizeOverLifetime.AddMember("enabled", value, alloc);
	ol.AddMember("Size", sizeOverLifetime, alloc);

	rapidjson::Value speedOverLifetime;
	speedOverLifetime.SetObject();
	value.SetBool(mySpeedOverLifetimeEnabled);
	mySpeedOverLifetime.ParseToJsonObject(speedOverLifetime, output, "Curve");
	speedOverLifetime.AddMember("enabled", value, alloc);
	ol.AddMember("Speed", speedOverLifetime, alloc);

	rapidjson::Value roationOverLifetime;
	roationOverLifetime.SetObject();
	value.SetBool(myRotationOverLifetimeEnabled);
	myRotationOverLifetime.ParseToJsonObject(roationOverLifetime, output, "Curve");
	roationOverLifetime.AddMember("enabled", value, alloc);
	ol.AddMember("Rotation", roationOverLifetime, alloc);


	rapidjson::Value colorOverLifetime;
	colorOverLifetime.SetObject();
	value.SetBool(myColorOverLifetimeEnabled);
	colorOverLifetime.AddMember("enabled", value, alloc);
	myColorOverLifetime.ParseToJsonObject(colorOverLifetime, output, "Gradient");
	ol.AddMember("Color", colorOverLifetime, alloc);

	rapidjson::Value VelocityOverLifetime;
	VelocityOverLifetime.SetObject();
	value.SetBool(myVelocityOverLifetimeEnabled);
	VelocityOverLifetime.AddMember("enabled", value, alloc);
	myVelocityOverLifetimeX.ParseToJsonObject(VelocityOverLifetime, output, "X");
	myVelocityOverLifetimeY.ParseToJsonObject(VelocityOverLifetime, output, "Y");
	myVelocityOverLifetimeZ.ParseToJsonObject(VelocityOverLifetime, output, "Z");
	value.SetFloat(myVelocityOverTimeInfluence);
	VelocityOverLifetime.AddMember("Strength", value, alloc);
	ol.AddMember("Velocity", VelocityOverLifetime, alloc);

#pragma endregion

#pragma region Graphics
	rapidjson::Value graphics;
	graphics.SetObject();
	value.SetInt(myBlendState);
	graphics.AddMember("Blend state", value, alloc);
	value.SetInt(myType);
	graphics.AddMember("Type", value, alloc);

	if (myType == Type::Flipbook)
	{
		value.SetInt(myFlipbookData->columns);
		graphics.AddMember("Columns", value, alloc);
		value.SetInt(myFlipbookData->rows);
		graphics.AddMember("Rows", value, alloc);
		value.SetInt(myFlipbookData->framerate);
		graphics.AddMember("Framerate", value, alloc);
		value.SetBool(myFlipbookData->loop);
		graphics.AddMember("Loop", value, alloc);
	}

	value.SetString(myTexture->GetName().c_str(), alloc);
	graphics.AddMember("Texture", value, alloc);
	output.AddMember("Graphics", graphics, alloc);

#pragma endregion

	DWORD attributes = GetFileAttributesA(myPathStr.c_str());
	if (attributes & FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributesA(myPathStr.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
	}

	std::ofstream ofs(myPath);
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	output.Accept(writer);
}


void ParticleEmitterSettings::LoadFromPath(const char* /*aPath*/)
{
	FILE* fp;
	bool error = fopen_s(&fp, myPath.string().c_str(), "rb");
	assert(!error && "Could not open file path");

	rapidjson::Document document;
	char readbuffer[16384]{};
	rapidjson::FileReadStream is(fp, readbuffer, sizeof(readbuffer));
	document.ParseStream(is);
	fclose(fp);

	if (document.HasMember("Version")) myVersion = document["Version"].GetInt();


#pragma region Emission
	rapidjson::GenericObject<false, rapidjson::Value> ed = document["Emission Data"].GetObj();
	mySpeed.LoadFromJson(ed);
	mySize.LoadFromJson(ed);
	myLifetime.LoadFromJson(ed);
	myTimeBetweenEmissions.LoadFromJson(ed);

	myDuration = ed["Duration"].GetFloat();
	myIsLooping = ed["Looping"].GetBool();
	myMaxParticles = ed["Max particles"].GetInt();
	myVertexBufferDesc.ByteWidth = static_cast<UINT>(myMaxParticles * static_cast<UINT>(sizeof(ParticleBufferData)));
	myLifetime.LoadFromJson(ed);
	myParticlesPerEmission.LoadFromJson(ed);
	myStartColor.LoadFromJson(ed["Start color"]);

	myUseGravity = ed["Use gravity"].GetBool();
	myGravityModifier = ed["Gravity modifier"].GetFloat();


#pragma endregion

#pragma region Shape
	if (myVersion >= 2)
	{
		rapidjson::GenericObject<false, rapidjson::Value> sd = document["Shape Data"].GetObj();
		myShape = (EmissionShape)sd["Shape"].GetInt();
	}

#pragma endregion

#pragma region Over lifetime

	rapidjson::GenericObject<false, rapidjson::Value> ot = document["Over lifetime"].GetObj();
	mySizeOverLifetime.LoadFromJson(ot["Size"]["Curve"]);
	mySizeOverLifetimeEnabled = ot["Size"]["enabled"].GetBool();
	mySpeedOverLifetime.LoadFromJson(ot["Speed"]["Curve"]);
	mySpeedOverLifetimeEnabled = ot["Speed"]["enabled"].GetBool();
	myColorOverLifetime.LoadFromJson(ot["Color"]["Gradient"]);
	myColorOverLifetimeEnabled = ot["Color"]["enabled"].GetBool();

#pragma endregion

#pragma region Graphics


	rapidjson::GenericObject<false, rapidjson::Value> graphics = document["Graphics"].GetObj();
	myBlendState = (BlendState)graphics["Blend state"].GetInt();

	if (myVersion >= 1)
	{
		std::string textureName = graphics["Texture"].GetString();
		myTexture = AssetRegistry::GetInstance()->GetAsset<Texture>(textureName);
		if (myTexture == nullptr) myTexture = AssetRegistry::GetInstance()->GetAsset<Texture>("Particle-Default");

		if (myVersion >= 3)
		{
			myType = (Type)graphics["Type"].GetInt();

			if (myVersion >= 4)
			{
				if (myType == Type::Flipbook)
				{
					myFlipbookData->columns = graphics["Columns"].GetInt();
					myFlipbookData->rows = graphics["Rows"].GetInt();
					myFlipbookData->framerate = graphics["Framerate"].GetInt();
					myFlipbookData->loop = graphics["Loop"].GetBool();
				}

				if (myVersion >= 5)
				{
					myUseWorldSpace = ed["WorldSpace"].GetBool();
					if (myVersion >= 6)
					{
						myStartDelay = ed["Delay"].GetFloat();
						rapidjson::GenericObject<false, rapidjson::Value> dir = ot["Velocity"].GetObj();
						myVelocityOverLifetimeEnabled = dir["enabled"].GetBool();
						myVelocityOverLifetimeX.LoadFromJson(dir["X"]);
						myVelocityOverLifetimeY.LoadFromJson(dir["Y"]);
						myVelocityOverLifetimeZ.LoadFromJson(dir["Z"]);
						myVelocityOverTimeInfluence = dir["Strength"].GetFloat();

						if (myVersion >= 7)
						{
							myRotationOverLifetime.LoadFromJson(ot["Rotation"]["Curve"]);
							myRotationOverLifetimeEnabled = ot["Rotation"]["enabled"].GetBool();

							if (ed.HasMember("Rotation"))
							{
								myRotation.LoadFromJson(ed);
							}

						}
					}
				}
			}
		}
	}

#pragma endregion
}

void ParticleEmitterSettings::RenderEmissionSettings()
{
	if (ImGui::CollapsingHeader(("Emission##" + myRuntimeId).c_str()))
	{
		bool edit = false;
		edit |= Catbox::Checkbox("World space", &myUseWorldSpace);
		edit |= Catbox::Checkbox("Looping", &myIsLooping);
		ImGui::PushItemWidth(100);
		edit |= Catbox::InputFloat("Duration", &myDuration);
		edit |= Catbox::InputFloat("Start delay", &myStartDelay);

		RenderDataPropertyOptions(&myTimeBetweenEmissions);
		RenderDataPropertyOptions(&myParticlesPerEmission);
		ImGui::PushItemWidth(100);
		if (Catbox::InputInt("Max particles", &myMaxParticles))
		{
			myNeedsRestart = true;
			myShouldUpdate = true;
			myVertexBufferDesc.ByteWidth = static_cast<UINT>(myMaxParticles * static_cast<UINT>(sizeof(ParticleBufferData)));
		}
		ImGui::Spacing();
		ImGui::Text("Particle Data");
		RenderDataPropertyOptions(&myLifetime);
		RenderDataPropertyOptions(&mySpeed, -100, 100);
		RenderDataPropertyOptions(&mySize);
		RenderDataPropertyOptions(&myRotation, -360, 360);
		edit |= Catbox::Checkbox("Use gravity", &myUseGravity);
		edit |= Catbox::DragFloat("Gravity modifier", &myGravityModifier, 0.01f, -10, 10);


		bool colorChanged = false;
		float startColor[4] = { myStartColor.r, myStartColor.g, myStartColor.b, myStartColor.a };
		edit |= colorChanged |= ImGui::ColorEdit3("Start color", startColor);

		if (colorChanged) myStartColor = startColor;
		myShouldUpdate |= edit;
		myHasUnsavedChanges |= edit;
	}
}

void ParticleEmitterSettings::RenderShapeSettings()
{
	if (ImGui::CollapsingHeader(("Shape##" + myRuntimeId).c_str()))
	{
		int shapeIndex = (int)myShape;
		const char* shapes[] = { "Cone", "Sphere", "Edge" };
		ImGui::PushItemWidth(130);
		if (ImGui::Combo("Shape", &shapeIndex, shapes, IM_ARRAYSIZE(shapes)))
		{
			myShape = (EmissionShape)shapeIndex;
			myHasUnsavedChanges = true;
		}
	}
}

void ParticleEmitterSettings::RenderOverLifetimeSettings()
{
	if (ImGui::CollapsingHeader(("Over lifetime##" + myRuntimeId).c_str()))
	{
#pragma region Color
		bool enabled = myColorOverLifetimeEnabled;

		if (Catbox::Checkbox(("##EnableCOL" + myRuntimeId).c_str(), &enabled))
		{
			myColorOverLifetimeEnabled = enabled;
			myHasUnsavedChanges = true;
		}
		ImGui::SameLine();

		if (!enabled) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));


		if (ImGui::TreeNodeEx(("Color over lifetime##" + myRuntimeId).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			myHasUnsavedChanges |= myColorOverLifetime.GradientEditor(draggingMark, selectedMark);
			if (selectedMark != nullptr && Input::GetKeyPress(KeyCode::DELETE_BUTTON))
			{
				myColorOverLifetime.RemoveColor(*selectedMark);
				selectedMark = nullptr;
			}

			ImGui::TreePop();
		}
		if (!enabled) ImGui::PopStyleColor();
#pragma endregion

#pragma region Size

		enabled = mySizeOverLifetimeEnabled;
		if (Catbox::Checkbox(("##EnableSIZE" + myRuntimeId).c_str(), &enabled))
		{
			mySizeOverLifetimeEnabled = enabled;
			myHasUnsavedChanges = true;
		}
		ImGui::SameLine();

		if (!enabled) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		if (ImGui::TreeNodeEx(("Size over lifetime##" + myRuntimeId).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			mySizeOverLifetime.RenderInProperties("");
			ImGui::TreePop();
		}
		if (!enabled) ImGui::PopStyleColor();

#pragma endregion

#pragma region Speed
		enabled = mySpeedOverLifetimeEnabled;
		if (Catbox::Checkbox(("##EnableSPEED" + myRuntimeId).c_str(), &enabled))
		{
			mySpeedOverLifetimeEnabled = enabled;
			myHasUnsavedChanges = true;
		}
		ImGui::SameLine();

		if (!enabled) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		if (ImGui::TreeNodeEx(("Speed over lifetime##" + myRuntimeId).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			mySpeedOverLifetime.RenderInProperties("");
			ImGui::TreePop();
		}
		if (!enabled) ImGui::PopStyleColor();

#pragma endregion

#pragma region Rotation

		enabled = myRotationOverLifetimeEnabled;
		if (Catbox::Checkbox(("##EnableROTATION" + myRuntimeId).c_str(), &enabled))
		{
			myRotationOverLifetimeEnabled = enabled;
			myHasUnsavedChanges = true;
		}
		ImGui::SameLine();
		if (!enabled) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		if (ImGui::TreeNodeEx(("Rotation over lifetime##" + myRuntimeId).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			myRotationOverLifetime.RenderInProperties("");
			ImGui::TreePop();
		}
		if (!enabled) ImGui::PopStyleColor();

#pragma endregion

#pragma region Movement
		enabled = myVelocityOverLifetimeEnabled;
		if (Catbox::Checkbox(("##EnableDIR" + myRuntimeId).c_str(), &enabled))
		{
			myVelocityOverLifetimeEnabled = enabled;
			myHasUnsavedChanges = true;
		}
		ImGui::SameLine();

		if (!enabled) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		if (ImGui::TreeNodeEx(("Velocity over lifetime##" + myRuntimeId).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			Catbox::DragFloat("Influence", &myVelocityOverTimeInfluence);
			ImGui::Text("X");
			myVelocityOverLifetimeX.RenderInProperties("X");
			ImGui::Text("Y");
			myVelocityOverLifetimeY.RenderInProperties("Y");
			ImGui::Text("Z");
			myVelocityOverLifetimeZ.RenderInProperties("Z");
			ImGui::TreePop();
		}
		if (!enabled) ImGui::PopStyleColor();
#pragma endregion
	}
}

void ParticleEmitterSettings::RenderGraphicsSettings()
{
	if (ImGui::CollapsingHeader(("Graphics##" + myRuntimeId).c_str()))
	{
		ImGui::Spacing();
		const char* typeNames[3] = { "Default", "Flipbook", "AtlasWalk" };

		int index = static_cast<int>(myType);
		if (ImGui::Combo("Type", &index, typeNames, 3))
		{
			myType = (Type)index;
		}
		ImGui::Spacing();

		ImGui::Image(static_cast<ImTextureID>(myTexture->GetShaderResourceView().Get()), { 64, 64 });
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
		ImGui::SetNextItemWidth(100);
		bool edit = false;
		std::shared_ptr<Texture> newTex = AssetRegistry::GetInstance()->AssetDropdownMenu<Texture>(myTexture.get(), "Texture", edit);
		if (edit)
		{
			myTexture = newTex;
		}


		int stateIndex = (int)myBlendState;
		const char* states[] = { "None", "Alpha Blend", "Additive" };
		ImGui::PushItemWidth(130);
		if (ImGui::Combo("Blend State", &stateIndex, states, IM_ARRAYSIZE(states)))
		{
			myBlendState = (BlendState)stateIndex;
			myHasUnsavedChanges = true;
		}

		if (myType == Type::Flipbook)
		{
			myFlipbookShader->RenderInProperties();
		}
		else if (myType == Type::AtlasWalk)
		{
			myAtlasWalkShader->RenderInProperties();
		}
	}
}


void ParticleEmitterSettings::RenderDataPropertyOptions(ParticleProperty<float>* aProperty, const float& aMin, const float& aMax)
{
	bool edit = false;
	ImGui::PushItemWidth(100);
	switch (aProperty->GetEvaluationType())
	{
	case EvaluationType::Default:
		edit |= Catbox::DragFloat(aProperty->myName.c_str(), &aProperty->myDefaultValue, 0.1f, aMin, aMax);
		break;

	case EvaluationType::Curve:
		ImGui::Text(aProperty->myName.c_str());
		break;

	case EvaluationType::Random:
	case EvaluationType::ValueOverTime:
		ImGui::Text(aProperty->myName.c_str());
		edit |= Catbox::DragFloat("Min", &aProperty->myRange.min, 0.1f, aMin, aMax);
		edit |= Catbox::DragFloat("Max", &aProperty->myRange.max), 0.1f, aMin, aMax;
		break;
	}

	myShouldUpdate |= edit;
	myHasUnsavedChanges |= edit;
}

void ParticleEmitterSettings::RenderDataPropertyOptions(ParticleProperty<int>* aProperty)
{
	bool edit = false;
	ImGui::PushItemWidth(100);
	switch (aProperty->GetEvaluationType())
	{
	case EvaluationType::Default:
		edit |= Catbox::DragInt(aProperty->myName.c_str(), &aProperty->myDefaultValue);
		break;

	case EvaluationType::Random:
	case EvaluationType::ValueOverTime:
		ImGui::Text(aProperty->myName.c_str());
		edit |= Catbox::DragInt("Min", &aProperty->myRange.min);
		edit |= Catbox::DragInt("Max", &aProperty->myRange.max);
		break;
	}

	myShouldUpdate |= edit;
	myHasUnsavedChanges |= edit;
}