#include "stdafx.h"
#include <ComponentTools/SceneManager.h>
#include "GraphicsEngine.h"
#include <shellapi.h>

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "Buffers/Buffers.h"
#include "DXDeviceStates.h"
#include "Editor\Windows\EditorWindowHandler.h"
#include "Editor\Windows\SceneWindow.h"
#include <ImGuizmo.h>
#include "Assets/Shader.h"
#include "Components\3D\ModelInstance.h"
#include "Components/UI/UISprite.h"

#include "Renderers/ForwardRenderer.h"
#include "Renderers/DeferredRenderer.h"
#include "Renderers/PostProcessRenderer.h"
#include "Renderers/UIRenderer.h"
#include "Renderers\DebugRenderer.h"
#include "Renderers\ShadowRenderer.h"
#include "Buffers\Buffers.h"

std::shared_ptr<VertexShader> GraphicsEngine::FullscreenVS;
GraphicsEngine* GraphicsEngine::Instance;

bool GraphicsEngine::Initialize(unsigned someX, unsigned someY,
	unsigned someWidth, unsigned someHeight,
	bool enableDeviceDebug, bool isEditorMode, HICON anIcon)
{
	myIsEditorMode = isEditorMode;
	Instance = this;

	// Initialize our window:
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = GraphicsEngine::WinProc;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"Catbox";
	if (isEditorMode)
	{
		windowClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);	//Fuck that Flashbang mf
	}
	RegisterClass(&windowClass);
	if (isEditorMode)
	{
		myWindowHandle = CreateWindow(
			L"Catbox",
			L"Catbox",
			WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,
			someX,
			someY,
			someWidth,
			someHeight,
			nullptr, nullptr, nullptr,
			this
		);
	}
	else
	{
		myWindowHandle = CreateWindow(
			L"Catbox",
			L"Catbox",
			WM_NCHITTEST | WS_POPUP | WS_VISIBLE,
			someX,
			someY,
			someWidth,
			someHeight,
			nullptr, nullptr, nullptr,
			this
		);
	}
	ShowWindow(myWindowHandle, SW_MAXIMIZE);
	bool isWindowed = true;
	if (isEditorMode)
	{
		DragAcceptFiles(myWindowHandle, true);
	}

	SendMessage(myWindowHandle, WM_SETICON, ICON_SMALL, (LPARAM)anIcon);
	SendMessage(myWindowHandle, WM_SETICON, ICON_BIG, (LPARAM)anIcon);

	if (isEditorMode)
	{
		Engine::GetInstance()->SetWindowSize(someWidth - (16 * isWindowed), someHeight - (39 * isWindowed));
	}
	else
	{
		Engine::GetInstance()->SetWindowSize(someWidth, someHeight);
	}
	Engine::GetInstance()->SetWindowSize(someWidth, someHeight);
	myFramework.Initialize(myWindowHandle, enableDeviceDebug, someWidth, someHeight);
	if (!myIsEditorMode)
	{
		DX11::SwapChain->SetFullscreenState(TRUE, nullptr);
	}

	myForwardRenderer = std::make_shared<ForwardRenderer>();
	myUIRenderer = std::make_shared<UIRenderer>();
	myDeferredRenderer = std::make_shared<DeferredRenderer>();
	myPostProcessRenderer = std::make_shared<PostProcessRenderer>();
	myDebugRenderer = std::make_shared<DebugRenderer>();
	myShadowRenderer = std::make_shared<ShadowRenderer>();

	myBuffers = std::make_shared<Buffers>();
	if (!myBuffers->Initialize()) return false;

	if (!BlendStates::InitBlendStates()) return false;
	if (!DepthStencilStates::InitDepthStencilStates()) return false;
	if (!SamplerStates::InitSamplerStates()) return false;
	if (!RastertizerStates::InitRastertizerStates()) return false;

	myPostProcessTarget = std::make_unique<Texture>();
	myPostProcessTarget->CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT);
	myPostProcessTarget->CreateRenderTargetView();

	myIntermediateTexture = std::make_unique<Texture>();
	myIntermediateTexture->CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT);
	myIntermediateTexture->CreateRenderTargetView();

	myPreviousDepthTexture = std::make_shared<Texture>();
	myPreviousDepthTexture->CreateScreenSizeTexture(DXGI_FORMAT_R32_FLOAT);
	myPreviousDepthTexture->CreateRenderTargetView();

	myLightMap = std::make_shared<Texture>();
	myLightMap->CreateScreenSizeTexture(DXGI_FORMAT_R32G32B32A32_FLOAT);
	myLightMap->CreateRenderTargetView();


#pragma region Imgui setup

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::GetStyle().WindowRounding = 0.0f;
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(myWindowHandle);
	ImGui_ImplDX11_Init(myFramework.Device.Get(), myFramework.Context.Get());
	/*Load Fonts
	- If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	- AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	- If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	- The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	- Read 'docs/FONTS.md' for more instructions and details.
	- Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
   io.Fonts->AddFontDefault();
   io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
   io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
   io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
   io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
   ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
   IM_ASSERT(font != NULL);*/

   // Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#pragma endregion

	bool lockMouse = !myIsEditorMode;
	if (lockMouse) 
	{
		Input::HideMouse();
		Input::Init(myWindowHandle);
		RECT WindowRect;
		GetWindowRect(myWindowHandle, &WindowRect);
		ClipCursor(&WindowRect);
	}

	if (isEditorMode)
	{
		SetClassLongPtr(myWindowHandle, GCLP_HBRBACKGROUND, NULL); //To not keep the Anti-Flashbang colour when moving the window
	}

	return true;
}

bool GraphicsEngine::Start()
{
	CreateShaders();
	if (!myForwardRenderer->Initialize(myBuffers)) return false;
	if (!myDeferredRenderer->Initialize(myBuffers)) return false;
	if (!myPostProcessRenderer->Initialize(myBuffers)) return false;
	if (!myDebugRenderer->Initialize(myBuffers)) return false;
	if (!myShadowRenderer->Initialize(myBuffers)) return false;
	if (!myUIRenderer->Initialize()) return false;
	return true;
}

void GraphicsEngine::CreateShaders()
{
	FullscreenVS = CreateAsset<VertexShader>("Resources/BuiltIn/Shaders/FullscreenVS.sh");
	std::string shaderData;
	ifstream vsFile;
	vsFile.open("Resources/BuiltIn/Shaders/FullscreenVS.cso", ios::binary);
	shaderData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &FullscreenVS->vertexShader);
	assert(!FAILED(result) && "Loading gbuffer vertex shader failed!");
	vsFile.close();
	D3D11_INPUT_ELEMENT_DESC fullscreenLayout[] =
	{
		{"SV_TARGET", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SV_TARGET", 1, DXGI_FORMAT_R16G16B16A16_SNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SV_TARGET", 2, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SV_TARGET", 3, DXGI_FORMAT_R16G16B16A16_SNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SV_TARGET", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SV_TARGET", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SV_TARGET", 6, DXGI_FORMAT_R8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	result = DX11::Device->CreateInputLayout(fullscreenLayout, sizeof(fullscreenLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC), shaderData.data(), shaderData.size(), FullscreenVS->inputLayout.GetAddressOf());

	myCopyPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/CopyTexturePS.sh");
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/CopyTexturePS.cso", ios::binary);
	string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myCopyPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading copy pixel shader failed!");

	myGammaCorrectionPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/GammaCorrectionPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/GammaCorrectionPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myGammaCorrectionPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading gamma correction pixel shader failed!");

	myBackgroundPS = CreateAsset<PixelShader>("Resources/BuiltIn/Shaders/SolidBackgroundPS.sh");
	psFile.open("Resources/BuiltIn/Shaders/SolidBackgroundPS.cso", ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myBackgroundPS->pixelShader);
	psFile.close();
	assert(!FAILED(result) && "Loading background pixel shader failed!");

	myOutlinePS = AssetRegistry::GetInstance()->GetAsset<PixelShader>("OutlinePS");
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GraphicsEngine::WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	// We want to be able to access the Graphics Engine instance from inside this function.
	static GraphicsEngine* graphicsEnginePtr = nullptr;
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;	
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	else if (uMsg == WM_CREATE)
	{

		const CREATESTRUCT* createdStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		graphicsEnginePtr = static_cast<GraphicsEngine*>(createdStruct->lpCreateParams);
	}
	else if (uMsg == WM_SIZE)
	{
		if (DX11::SwapChain) {
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			DX11::ResizeBackBuffer(width, height);
		}
	}
	else if (uMsg == WM_DROPFILES)
	{
		HDROP hDropInfo = (HDROP)wParam;
		char path[MAX_PATH];
		hDropInfo;
		DragQueryFileA(hDropInfo, 0, (LPSTR)path, sizeof(path));
		Editor::GetInstance()->DropFile(path);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void GraphicsEngine::BeginFrame()
{
	// F1 - This is where we clear our buffers and start the DX frame.
	//myFramework.BeginFrame({ 0.7f, 0.5f, 0.6f, 1 });

	myFramework.BeginFrame({ 0.1f, 0.1f, 0.1f, 1 });
	if (myIsEditorMode)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		ImVec2 vWindowSize = ImGui::GetMainViewport()->Size;
		ImVec2 vPos0 = ImGui::GetMainViewport()->Pos;

		ImGui::SetNextWindowPos(ImVec2((float)vPos0.x, (float)vPos0.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2((float)vWindowSize.x, (float)vWindowSize.y), 1);

		ImGui::Begin("##Editor", nullptr,
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoTitleBar
		);

		static const ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiID dockSpace = ImGui::GetID("##Editor");
		ImGui::DockSpace(dockSpace, ImVec2(0.0f, 0.0f), dockspaceFlags);
	}
}

void GraphicsEngine::RenderFrame()
{
	if (Input::GetKeyPress(KeyCode::F6))
	{
		if (myDebugRenderMode == 10)
		{
			myDebugRenderMode = 0;
		}
		else
		{
			myDebugRenderMode++;
		}
		myForwardRenderer->SetDebugRenderMode(myDebugRenderMode);
	}
	if (EDITORMODE)
	{
		Editor::GetInstance()->Render();

		if (!PLAYMODE && !Editor::GetInstance()->IsPrefabBrushMode())
		{
			for (size_t i = 0; i < myMeshesToRenderDeferred.size(); i++)
			{
				if (myMeshesToRenderDeferred[i]->GetGameObject().IsSelected(0))
				{
					myMeshesToRenderOutline.push_back(myMeshesToRenderDeferred[i]);
				}
			}
			for (size_t i = 0; i < myMeshesToRenderForward.size(); i++)
			{
				if (myMeshesToRenderForward[i]->GetGameObject().IsSelected(0))
				{
					myMeshesToRenderOutline.push_back(myMeshesToRenderForward[i]);
				}
			}
		}
	}

	if (PLAYMODE)
	{
		for (size_t i = 0; i < myMeshesToRenderDeferred.size(); i++)
		{
			if (myMeshesToRenderDeferred[i]->GetGameObject().GetRenderOutline())
			{
				myMeshesToRenderOutlinePlayMode.push_back(myMeshesToRenderDeferred[i]);
			}
		}
		for (size_t i = 0; i < myMeshesToRenderForward.size(); i++)
		{
			if (myMeshesToRenderForward[i]->GetGameObject().GetRenderOutline())
			{
				myMeshesToRenderOutlinePlayMode.push_back(myMeshesToRenderForward[i]);
			}
		}
	}

	if (!myCamera)
	{
		printerror("No camera set!");
		if (EDITORMODE) ImGui::End();
		return;
	}

	DX11::Context->ClearDepthStencilView(myCamera->GetDepthStencil().GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	Texture* targetToUse = &myCamera->GetRenderTexture();
	Texture* resourceToUse = myIntermediateTexture.get();
	//DX11::Context->OMSetRenderTargets(1, &renderViewTexture->GetRenderTargetView(), DX11::DepthBuffer.Get());

	if (!myCamera)
	{
		printerror("No camera set!");
		if (EDITORMODE) ImGui::End();
		return;
	}

	float clearColor[4] = { myClearColor.r, myClearColor.g, myClearColor.b, 1 };
	DX11::Context->ClearRenderTargetView(targetToUse->GetRenderTargetView().Get(), clearColor);
	DX11::Context->ClearRenderTargetView(myLightMap->GetRenderTargetView().Get(), clearColor);
	DX11::Context->ClearRenderTargetView(myIntermediateTexture->GetRenderTargetView().Get(), clearColor);
	DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_AlphaBlend)].Get(), nullptr, 0xffffffff);
	if (PLAYMODE || !Editor::GetInstance()->IsUIEditMode())
	{
		myBuffers->CreateLightsBuffer(mySceneLightData);

		DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[DepthStencilState::DSS_ReadWrite].Get(), 0xffffffff);
		//Create shadow maps
		for (size_t i = 0; i < myShadowsToRender.size(); i++)
		{
			if (!myShadowsToRender[i]->GetLightData().castShadows) continue;
			DX11::Context->ClearDepthStencilView(myShadowsToRender[i]->GetShadowMap()->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		myShadowRenderer->Render(myMeshesToRenderDeferred, myShadowsToRender);
		myShadowRenderer->Render(myMeshesToRenderForward, myShadowsToRender);
		myBuffers->CreateFrameBuffer(myCamera);

		//Render the scene to the camera's texture
		myDeferredRenderer->GenerateGBuffer(myMeshesToRenderDeferred);
		myPostProcessRenderer->RenderAmbientOcclusion(*myCamera, resourceToUse->GetShaderResourceView().GetAddressOf());

		myDeferredRenderer->Render(targetToUse->GetRenderTargetView());

		DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[DepthStencilState::DSS_ReadWrite].Get(), 0xffffffff);

		myForwardRenderer->Render(myMeshesToRenderForward, myParticleSystemsToRender, targetToUse->GetRenderTargetView());
		//RunFullScreenShader(myCamera->GetDepthStencil().GetShaderResourceView().GetAddressOf(), myPreviousDepthTexture->GetRenderTargetView().GetAddressOf(), myCopyPS);
		//DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[DepthStencilState::DSS_Off].Get(), 0xffffffff);
		std::swap(targetToUse, resourceToUse);

		myBuffers->CreateFrameBuffer(myCamera);
		//Add post processing onto the camera's texture
		if (myCamera->IsPostProcessingEnabled() && myCamera->GetPostProcessingVolume()->GetPostProcess())
		{
			myPostProcessRenderer->RenderBloom(*myCamera, resourceToUse->GetShaderResourceView().GetAddressOf(), targetToUse->GetRenderTargetView().GetAddressOf());
			std::swap(targetToUse, resourceToUse);
		}

		if (myGammaCorrectEnabled)
		{
			RunFullScreenShader(resourceToUse->GetShaderResourceView().GetAddressOf(), targetToUse->GetRenderTargetView().GetAddressOf(), myGammaCorrectionPS);
			std::swap(targetToUse, resourceToUse);
		}

		if (myCamera->IsPostProcessingEnabled() && myCamera->GetPostProcessingVolume()->GetPostProcess())
		{
			myPostProcessRenderer->RenderPostProcessing(*myCamera, resourceToUse, targetToUse);
			std::swap(targetToUse, resourceToUse);
		}

		if (EDITORMODE && !myMeshesToRenderOutline.empty())
		{
			Texture* mask = myPostProcessRenderer->CreateMask(myMeshesToRenderOutline);
			myPostProcessRenderer->RenderOutlines(mask->GetShaderResourceView().GetAddressOf(), resourceToUse->GetShaderResourceView().GetAddressOf(), targetToUse->GetRenderTargetView().GetAddressOf(), true);
			std::swap(targetToUse, resourceToUse);
		}

		if (PLAYMODE && !myMeshesToRenderOutlinePlayMode.empty())
		{
			Texture* mask = myPostProcessRenderer->CreateMask(myMeshesToRenderOutlinePlayMode);
			myPostProcessRenderer->RenderOutlines(mask->GetShaderResourceView().GetAddressOf(), resourceToUse->GetShaderResourceView().GetAddressOf(), targetToUse->GetRenderTargetView().GetAddressOf(), false);
			std::swap(targetToUse, resourceToUse);
		}

		if (PLAYMODE || Editor::GetInstance()->IsUIVisible())
		{
			DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_AlphaBlend)].Get(), nullptr, 0xffffffff);
			myUIRenderer->Render(mySpritesToRender, resourceToUse->GetRenderTargetView());
			DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_None)].Get(), nullptr, 0xffffffff);
		}
	}
	else
	{
		DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_AlphaBlend)].Get(), nullptr, 0xffffffff);
		myUIRenderer->Render(mySpritesToRender, resourceToUse->GetRenderTargetView());
		DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_None)].Get(), nullptr, 0xffffffff);
	}

	if (EDITORMODE)
	{
		myDebugRenderer->Render(myNavmeshesToRender, resourceToUse->GetRenderTargetView());

		SceneWindow* viewport = Editor::GetInstance()->GetWindowHandler().GetAllWindowsOfType<SceneWindow>(WindowType::Scene)[0];
		Editor::GetInstance()->GetGizmos().Update(myCamera);
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> viewportTexture = viewport->GetTexture().GetRenderTargetView();
		DX11::Context->ClearRenderTargetView(viewportTexture.Get(), clearColor);

		if (myDebugDrawerToggle || !PLAYMODE)
		{
			Editor::GetInstance()->GetGizmos().RenderGizmos(viewport);

			myDebugRenderer->Render(myDebugLinesToRender, myDebugCubesToRender, resourceToUse->GetRenderTargetView());
			//Editor::GetInstance()->GetGizmos().DrawGrid(viewport);
		}

		myBuffers->myColorBufferData.Color = myClearColor;
		myBuffers->myColorBuffer.SetData(&myBuffers->myColorBufferData);
		DX11::Context->PSSetConstantBuffers(0, 1, myBuffers->myColorBuffer.GetAddress());
		RunFullScreenShader(resourceToUse->GetShaderResourceView().GetAddressOf(), viewportTexture.GetAddressOf(), myCopyPS);

		DX11::Context->OMSetRenderTargets(1, DX11::BackBuffer.GetAddressOf(), nullptr);
		ImGui::End();
		ImGui::Render();
		DX11::Context->OMSetBlendState(BlendStates::GetBlendStates()[static_cast<int>(BlendState::BS_None)].Get(), nullptr, 0xffffffff);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	else
	{
		DX11::Context->RSSetViewports(1, &DX11::viewPort);
		RunFullScreenShader(resourceToUse->GetShaderResourceView().GetAddressOf(), DX11::BackBuffer.GetAddressOf(), myCopyPS);
	}

	

	EmptyVectors();
	//myFramework.SwapChain->Present(1, 0); // Present with vsync
}

void GraphicsEngine::RunFullScreenShader(ID3D11ShaderResourceView* const* aSource, ID3D11RenderTargetView* const* aTarget, std::shared_ptr<PixelShader> aPixelShader)
{
	DX11::Context->OMSetDepthStencilState(DepthStencilStates::GetDepthStencilStates()[DepthStencilState::DSS_ReadOnly].Get(), 0xffffffff);
	ID3D11RenderTargetView* clearRtvs[1] = { nullptr };
	ID3D11ShaderResourceView* clearSrvs[1] = { nullptr };
	DX11::Context->PSSetShaderResources(0, 1, clearSrvs);
	DX11::Context->OMSetRenderTargets(1, clearRtvs, nullptr);

	DX11::Context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DX11::Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(nullptr);
	DX11::Context->VSSetShader(FullscreenVS->vertexShader, nullptr, 0);
	DX11::Context->PSSetShader(aPixelShader->pixelShader, nullptr, 0);
	DX11::Context->PSSetShaderResources(0, 1, aSource);
	DX11::Context->OMSetRenderTargets(1, aTarget, nullptr);
	DX11::Context->Draw(3, 0);

	DX11::Context->PSSetShaderResources(0, 1, clearSrvs);
	DX11::Context->OMSetRenderTargets(1, clearRtvs, nullptr);
}

void GraphicsEngine::EmptyVectors()
{
	for (size_t i = 0; i < myMeshesToRenderForward.size(); i++)
	{
		myMeshesToRenderForward[i]->GetModel()->ClearRenderedInstances();
	}
	for (size_t i = 0; i < myMeshesToRenderDeferred.size(); i++)
	{
		myMeshesToRenderDeferred[i]->GetModel()->ClearRenderedInstances();
	}

	myMeshesToRenderDeferred.clear();
	myMeshesToRenderForward.clear();
	myMeshesToRenderOutline.clear();
	myMeshesToRenderOutlinePlayMode.clear();
	myParticleSystemsToRender.clear();
	mySpritesToRender.clear();
	for (int i = 0; i < myDebugLinesToRender.size(); i++)
	{
		myDebugLinesToRender[i].aliveTime -= deltaTime;
		if (myDebugLinesToRender[i].aliveTime < 0) 
		{
			myDebugLinesToRender.erase(myDebugLinesToRender.begin() + i);
		}
	}
	myDebugCubesToRender.clear();
	myNavmeshesToRender.clear();
	myShadowsToRender.clear();
}

void GraphicsEngine::SetCamera(Camera* aCamera)
{
	myCamera = aCamera;
}

ForwardRenderer& GraphicsEngine::GetForwardRenderer()
{
	return *myForwardRenderer;
}

DeferredRenderer& GraphicsEngine::GetDeferredRenderer()
{
	return *myDeferredRenderer;
}

PostProcessRenderer& GraphicsEngine::GetPostProcessRenderer()
{
	return *myPostProcessRenderer;
}

void GraphicsEngine::EndFrame()
{
	// F1 - This is where we finish our rendering and tell the framework
	// to present our result to the screen.

	myFramework.EndFrame();
	if (myIsEditorMode)
	{
		ImGui::EndFrame();
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}

void GraphicsEngine::SetLights(SceneLightData* aSceneLightData)
{
	mySceneLightData = aSceneLightData;
}

void GraphicsEngine::SetEnvironmentLight(Light* anEnvLight)
{
	myBuffers->myEnvironmentLight = anEnvLight;
}

void GraphicsEngine::SetClearColor(const Color& aColor)
{
	myClearColor = aColor;
}

void GraphicsEngine::AddToRenderQueue(ModelInstance* aModel, Renderer aRendererToUse)
{
	if (aRendererToUse == Deferred)
	{
		myMeshesToRenderDeferred.push_back(aModel);
	}
	else if (aRendererToUse == Forward)
	{
		myMeshesToRenderForward.push_back(aModel);
	}
	else
	{

	}
}
void GraphicsEngine::AddToRenderQueue(ParticleSystem* aParticleSystem)
{
	myParticleSystemsToRender.push_back(aParticleSystem);
}

void GraphicsEngine::AddToRenderQueue(UISprite* aSprite)
{
	mySpritesToRender.push_back(aSprite);
}

void GraphicsEngine::AddToRenderQueue(DebugLine aLine)
{
	myDebugLinesToRender.push_back(aLine);
}

void GraphicsEngine::AddToRenderQueue(DebugCube aCube)
{
	myDebugCubesToRender.push_back(aCube);
}

void GraphicsEngine::AddToRenderQueue(NavMesh* aNavmesh)
{
	myNavmeshesToRender.push_back(aNavmesh);
}

void GraphicsEngine::AddToRenderQueue(Light* aShadowCastingLight)
{
	myShadowsToRender.push_back(aShadowCastingLight);
}

void GraphicsEngine::RenderOutline(ModelInstance* aModel)
{
	myMeshesToRenderOutline.push_back(aModel);
}

void GraphicsEngine::RemoveFromRenderQueue(ModelInstance* aModel)
{
	for (size_t i = 0; i < myMeshesToRenderDeferred.size(); i++)
	{
		if (myMeshesToRenderDeferred[i] == aModel)
		{
			myMeshesToRenderDeferred.erase(myMeshesToRenderDeferred.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < myMeshesToRenderForward.size(); i++)
	{
		if (myMeshesToRenderForward[i] == aModel)
		{
			myMeshesToRenderForward.erase(myMeshesToRenderForward.begin() + i);
			break;
		}
	}
}

void GraphicsEngine::RemoveFromRenderQueue(ParticleSystem* aParticleSystem)
{
	for (size_t i = 0; i < myParticleSystemsToRender.size(); i++)
	{
		if (myParticleSystemsToRender[i] == aParticleSystem)
		{
			myParticleSystemsToRender.erase(myParticleSystemsToRender.begin() + i);
			break;
		}
	}
}

void GraphicsEngine::RemoveFromRenderQueue(UISprite* aSprite)
{
	for (size_t i = 0; i < mySpritesToRender.size(); i++)
	{
		if (mySpritesToRender[i] == aSprite)
		{
			mySpritesToRender.erase(mySpritesToRender.begin() + i);
			break;
		}
	}
}