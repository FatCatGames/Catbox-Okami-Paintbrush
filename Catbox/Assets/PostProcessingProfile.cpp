#include "stdafx.h"
#include "PostProcessingProfile.h"
#include "CommonUtilities/UtilityFunctions.hpp"

void PostProcessingProfile::SaveAsset(const char* aPath)
{
	int version = 11;

	RapidJsonWrapper* wrapper = RapidJsonWrapper::GetInstance();
	auto& document = wrapper->CreateDocument();

	wrapper->SetTarget(document);
	wrapper->SaveValue<DataType::Int>("Version", version);

	auto colorAdjustments = wrapper->CreateObject();
	wrapper->SetTarget(colorAdjustments);
	wrapper->SaveValue<DataType::Bool>("Enabled", myEnabledEffects[PostProcessEffect::ColorAdjustment]);
	mySettings.Tint.ParseToJsonObject(colorAdjustments, document, "Tint");
	wrapper->SaveValue<DataType::Float>("Brightness", mySettings.Brightness);
	wrapper->SaveValue<DataType::Float>("Contrast", mySettings.Contrast);
	wrapper->SaveValue<DataType::Float>("Saturation", mySettings.Saturation);
	wrapper->SetTarget(document);
	wrapper->SaveObject("Color Adjustments", colorAdjustments);

	auto bloom = wrapper->CreateObject();
	wrapper->SetTarget(bloom);
	wrapper->SaveValue<DataType::Bool>("Enabled", myEnabledEffects[PostProcessEffect::Bloom]);
	wrapper->SaveValue<DataType::Float>("Strength", myBloomSettings.Strength);
	wrapper->SaveValue<DataType::Float>("Cutoff", myBloomSettings.Cutoff);
	wrapper->SaveValue<DataType::Float>("Blur", myBloomSettings.Blur);
	wrapper->SetTarget(document);
	wrapper->SaveObject("Bloom", bloom);

	auto vignette = wrapper->CreateObject();
	wrapper->SetTarget(vignette);
	wrapper->SaveValue<DataType::Bool>("Enabled", myEnabledEffects[PostProcessEffect::Vignette]);
	wrapper->SaveValue<DataType::Float>("Strength", myVignetteSettings.Strength);
	wrapper->SaveValue<DataType::Float>("Ratio", myVignetteSettings.Ratio);
	wrapper->SaveValue<DataType::Float>("Radius", myVignetteSettings.Radius);
	wrapper->SaveValue<DataType::Int>("Slope", myVignetteSettings.Slope);
	wrapper->SaveValue<DataType::Float>("CenterX", myVignetteSettings.Center.x);
	wrapper->SaveValue<DataType::Float>("CenterY", myVignetteSettings.Center.y);
	wrapper->SetTarget(document);
	wrapper->SaveObject("Vignette", vignette);

	auto ssao = wrapper->CreateObject();
	wrapper->SetTarget(ssao);
	wrapper->SaveValue<DataType::Bool>("Enabled", myEnabledEffects[PostProcessEffect::AmbientOcclusion]);
	wrapper->SaveValue<DataType::Float>("Bias", mySSAOSettings.bias);
	wrapper->SaveValue<DataType::Float>("Radius", mySSAOSettings.radius);
	wrapper->SaveValue<DataType::Float>("Strength", mySSAOSettings.strength);
	wrapper->SetTarget(document);
	wrapper->SaveObject("SSAO", ssao);

	auto fog = wrapper->CreateObject();
	wrapper->SetTarget(fog);
	wrapper->SaveValue<DataType::Bool>("Enabled", myEnabledEffects[PostProcessEffect::Fog]);
	myFogSettings.color.ParseToJsonObject(fog, document, "Color");
	wrapper->SaveValue<DataType::Float>("Cutoff", myFogSettings.cutoff);
	wrapper->SaveValue<DataType::Float>("Density", myFogSettings.density);
	myFogSettings.yDepthColor.ParseToJsonObject(fog, document, "YColor");
	wrapper->SaveValue<DataType::Float>("YCutoff", myFogSettings.yDepthCutoff);
	wrapper->SaveValue<DataType::Float>("YSmoothing", myFogSettings.yDepthSmoothing);
	wrapper->SaveValue<DataType::Float>("YDensity", myFogSettings.yDepthDensity);

	wrapper->SetTarget(document);
	wrapper->SaveObject("Fog", fog);

	wrapper->SaveDocument(aPath);
}

void PostProcessingProfile::LoadFromPath(const char* aPath)
{
	RapidJsonWrapper* wrapper = RapidJsonWrapper::GetInstance();
	rapidjson::Document& document = wrapper->LoadDocument(aPath);
	myVersion = wrapper->LoadValue("Version").GetInt();
	auto colorAdjustments = wrapper->LoadValue("Color Adjustments").GetObj();

	wrapper->SetTarget(colorAdjustments);
	mySettings.Brightness = wrapper->LoadValue("Brightness").GetFloat();
	mySettings.Contrast = wrapper->LoadValue("Contrast").GetFloat();
	mySettings.Saturation = wrapper->LoadValue("Saturation").GetFloat();
	wrapper->SetTarget(document);

	if (myVersion >= 1)
	{
		mySettings.Tint.LoadFromJson(colorAdjustments["Tint"].GetObj());
	}
	if (myVersion >= 3)
	{
		auto bloom = wrapper->LoadValue("Bloom").GetObj();

		wrapper->SetTarget(bloom);
		myBloomSettings.Strength = wrapper->LoadValue("Strength").GetFloat();
		myBloomSettings.Cutoff = wrapper->LoadValue("Cutoff").GetFloat();
		myBloomSettings.Blur = wrapper->LoadValue("Blur").GetFloat();
	}
	if (myVersion >= 4)
	{
		if (myVersion >= 5) myEnabledEffects[PostProcessEffect::Bloom] = wrapper->LoadValue("Enabled").GetBool();
		else myEnabledEffects[PostProcessEffect::Bloom] = wrapper->LoadValue("Bloom Enabled").GetBool();
	}
	if (myVersion >= 5)
	{
		wrapper->SetTarget(document);
		auto vignette = wrapper->LoadValue("Vignette").GetObj();

		wrapper->SetTarget(vignette);
		myVignetteSettings.Strength = wrapper->LoadValue("Strength").GetFloat();
		myEnabledEffects[PostProcessEffect::Vignette] = wrapper->LoadValue("Enabled").GetBool();

		wrapper->SetTarget(colorAdjustments);
		myEnabledEffects[PostProcessEffect::ColorAdjustment] = wrapper->LoadValue("Enabled").GetBool();

		if (myVersion >= 6)
		{
			wrapper->SetTarget(vignette);
			myVignetteSettings.Ratio = wrapper->LoadValue("Ratio").GetFloat();
			myVignetteSettings.Radius = wrapper->LoadValue("Radius").GetFloat();
			myVignetteSettings.Slope = wrapper->LoadValue("Slope").GetInt();
			myVignetteSettings.Center.x = wrapper->LoadValue("CenterX").GetFloat();
			myVignetteSettings.Center.y = wrapper->LoadValue("CenterY").GetFloat();
			myEnabledEffects[PostProcessEffect::Vignette] = wrapper->LoadValue("Enabled").GetBool();

			if (myVersion >= 7)
			{
				wrapper->SetTarget(document);
				auto ssao = wrapper->LoadValue("SSAO").GetObj();
				wrapper->SetTarget(ssao);

				mySSAOSettings.radius = wrapper->LoadValue("Radius").GetFloat();
				myEnabledEffects[PostProcessEffect::AmbientOcclusion] = wrapper->LoadValue("Enabled").GetBool();

				if (myVersion == 7)
				{
					mySSAOSettings.bias = wrapper->LoadValue("Intensity").GetFloat();
					mySSAOSettings.strength = wrapper->LoadValue("Bias").GetFloat();
				}
				else
				{
					mySSAOSettings.bias = wrapper->LoadValue("Bias").GetFloat();
					mySSAOSettings.strength = wrapper->LoadValue("Strength").GetFloat();
				}

				if (myVersion >= 9)
				{
					wrapper->SetTarget(document);
					auto fog = wrapper->LoadValue("Fog").GetObj();
					wrapper->SetTarget(fog);

					myFogSettings.density = wrapper->LoadValue("Density").GetFloat();
					myFogSettings.cutoff = wrapper->LoadValue("Cutoff").GetFloat();
					myEnabledEffects[PostProcessEffect::Fog] = wrapper->LoadValue("Enabled").GetBool();
					myFogSettings.color.LoadFromJson(fog["Color"].GetObj());

					if (myVersion >= 10)
					{
						myFogSettings.yDepthDensity = wrapper->LoadValue("YDensity").GetFloat();
						myFogSettings.yDepthCutoff = wrapper->LoadValue("YCutoff").GetFloat();
						myFogSettings.yDepthColor.LoadFromJson(fog["YColor"].GetObj());

						if (myVersion >= 11)
						{
							myFogSettings.yDepthSmoothing = wrapper->LoadValue("YSmoothing").GetFloat();
						}
					}
				}
			}
		}
	}

}

void PostProcessingProfile::RenderInProperties()
{
	Catbox::Checkbox("Color Adjustments", &myEnabledEffects[PostProcessEffect::ColorAdjustment]);
	if (myEnabledEffects[PostProcessEffect::ColorAdjustment])
	{
		float tint[4] = { mySettings.Tint.r, mySettings.Tint.g, mySettings.Tint.b, 1 };
		if (ImGui::ColorEdit3("Tint", tint))
		{
			mySettings.Tint = tint;

		}
		Catbox::DragFloat("Brightness", &mySettings.Brightness, 0.01f, 0, 10.f);
		Catbox::DragFloat("Contrast", &mySettings.Contrast, 0.05f, 0, 10.f);
		Catbox::DragFloat("Saturation", &mySettings.Saturation, 0.05f, 0, 10.f);
	}

	Catbox::Checkbox("Bloom", &myEnabledEffects[PostProcessEffect::Bloom]);
	if (myEnabledEffects[PostProcessEffect::Bloom])
	{
		Catbox::DragFloat("Strength##Bloom", &myBloomSettings.Strength, 0.01f, 0, 10.f);
		Catbox::DragFloat("Cutoff##Bloom", &myBloomSettings.Cutoff, 0.01f, 0.01f, 1);
		Catbox::DragFloat("Blur##Bloom", &myBloomSettings.Blur, 0.01f, 0, 1);
	}

	Catbox::Checkbox("Vignette", &myEnabledEffects[PostProcessEffect::Vignette]);
	if (myEnabledEffects[PostProcessEffect::Vignette])
	{
		ImGui::PushItemWidth(70);
		Catbox::DragFloat("Strength##Vignette", &myVignetteSettings.Strength, 0.1f, -10, 10);
		Catbox::DragFloat("Ratio##Vignette", &myVignetteSettings.Ratio, 0.01f, 0.15f, 16);
		Catbox::DragFloat("Radius##Vignette", &myVignetteSettings.Radius, 0.01f, -1, 3);
		Catbox::DragInt("Slope##Vignette", &myVignetteSettings.Slope, 2, 2, 16, "%.2f", ImGuiSliderFlags_NoInput);
		Catbox::DragFloat("Center X##Vignette", &myVignetteSettings.Center.x, 0.01f, -1, 2);
		ImGui::SameLine();
		Catbox::DragFloat("Center Y##Vignette", &myVignetteSettings.Center.y, 0.01f, -1, 2);
		ImGui::PopItemWidth();
	}

	Catbox::Checkbox("Ambient Occlusion", &myEnabledEffects[PostProcessEffect::AmbientOcclusion]);
	if (myEnabledEffects[PostProcessEffect::AmbientOcclusion])
	{
		Catbox::DragFloat("Bias##SSAO", &mySSAOSettings.bias, 0.01f, 0, 10);
		//Catbox::DragFloat("Scale##SSAO", &mySSAOSettings.scale, 0.01f, 0, 1);
		Catbox::DragFloat("Strength##SSAO", &mySSAOSettings.strength, 0.01f, 0, 10);
		Catbox::DragFloat("Radius##SSAO", &mySSAOSettings.radius, 0.01f, 0, 1);
	}

	Catbox::Checkbox("Fog", &myEnabledEffects[PostProcessEffect::Fog]);
	if (myEnabledEffects[PostProcessEffect::Fog])
	{
		ImGui::Text("Depth fog");
		float color[4] = { myFogSettings.color.r, myFogSettings.color.g, myFogSettings.color.b, 1 };
		if (ImGui::ColorEdit3("Color##Fog", color))
		{
			myFogSettings.color = color;
		}

		Catbox::DragFloat("Density##Fog", &myFogSettings.density, 0.01f, 0, 10);
		Catbox::DragFloat("Cutoff##Fog", &myFogSettings.cutoff, 0.01f, 0, 10);

		ImGui::Text("Height fog");
		float heightColor[4] = { myFogSettings.yDepthColor.r, myFogSettings.yDepthColor.g, myFogSettings.yDepthColor.b, 1 };
		if (ImGui::ColorEdit3("Color##HeightFog", heightColor))
		{
			myFogSettings.yDepthColor = heightColor;
		}

		Catbox::DragFloat("Density##HeightFog", &myFogSettings.yDepthDensity, 0.01f, 0, 1);
		Catbox::DragFloat("Cutoff##HeightFog", &myFogSettings.yDepthCutoff, 0.1f, -100, 100);
		Catbox::DragFloat("Smoothing##HeightFog", &myFogSettings.yDepthSmoothing, 0.1f, -100, 100);
	}

	if (ImGui::Button("Save", { 70, 30 }))
	{
		SaveAsset(myPath.string().c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete", { 70, 30 }))
	{
		AssetRegistry::GetInstance()->RemoveAsset<PostProcessingProfile>(myName);
	}
	if (ImGui::Button("Reload PostProcess shaders"))
	{
		myShouldReloadShaders = true;
	}

	//ImGui::End();
}