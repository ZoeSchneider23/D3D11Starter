#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStruct.h"

#include <DirectXMath.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

//test global
XMFLOAT3 pos1 = XMFLOAT3(0.0f, 0.0f, 0.0f);

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMaterials();
	CreateGeometry();

	//add constant buffer
	// Describe the constant buffer
	D3D11_BUFFER_DESC vcbDesc = {}; // Sets struct to all zeros
	vcbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vcbDesc.ByteWidth = (sizeof(VertexBufferStruct) + 15) / 16 * 16; // Must be a multiple of 16
	vcbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vcbDesc.Usage = D3D11_USAGE_DYNAMIC;
	Graphics::Device->CreateBuffer(&vcbDesc, 0, vertexConstantBuffer.GetAddressOf());

	D3D11_BUFFER_DESC pcbDesc = {}; // Sets struct to all zeros
	pcbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pcbDesc.ByteWidth = (sizeof(PixelBufferStruct) + 15) / 16 * 16; // Must be a multiple of 16
	pcbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pcbDesc.Usage = D3D11_USAGE_DYNAMIC;
	Graphics::Device->CreateBuffer(&pcbDesc, 0, pixelConstantBuffer.GetAddressOf());
	
	//Create constant buffer data
	vsData.world = XMFLOAT4X4 ();

	//Create Camera
	cams.push_back(std::make_shared<Camera>(XMFLOAT3(0.0f, 0.0f, -10.0f), Window::AspectRatio(), XM_PIDIV4));
	cams.push_back(std::make_shared<Camera>(XMFLOAT3(1.0f, 0.0f, -10.0f), Window::AspectRatio(), 1.0f));
	activeCam = 0;
	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		//Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		//Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
		
		// Initialize ImGui itself & platform/renderer backends
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
		// Pick a style (uncomment one of these 3)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();

	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::LoadVertexShader( ID3DBlob *vertexShaderBlob)
{
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	
	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
		vertexShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		vertexShader.GetAddressOf());

	vertexShaders.push_back(vertexShader);
}

void Game::LoadPixelShader(ID3DBlob* pixelShaderBlob)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
		pixelShaderBlob->GetBufferSize(),		// How big is that data?
		0,										// No classes in this shader
		pixelShader.GetAddressOf());

	pixelShaders.push_back(pixelShader);
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		
		// Create the actual Direct3D shaders on the GPU
					// Address of the ID3D11PixelShader pointer
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		LoadPixelShader(pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"DebugUVsPS.cso").c_str(), &pixelShaderBlob);
		LoadPixelShader(pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"DebugNormalsPS.cso").c_str(), &pixelShaderBlob);
		LoadPixelShader(pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"CustomPS.cso").c_str(), &pixelShaderBlob);
		LoadPixelShader(pixelShaderBlob);

		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);
		LoadVertexShader(vertexShaderBlob);
					// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[4] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "TEXCOORD";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;			// 4x 32-bit floats
		inputElements[2].SemanticName = "NORMAL";							// Match our vertex shader input!
		inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		inputElements[3].Format = DXGI_FORMAT_R32_FLOAT;			// 4x 32-bit floats
		inputElements[3].SemanticName = "TIME";							// Match our vertex shader input!
		inputElements[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element


		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			4,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}

void Game::CreateMaterials()
{
	//Create materials
	XMFLOAT4 col1 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mats.push_back(std::make_shared<Material>(col1, vertexShaders[0], pixelShaders[0]));
	mats.push_back(std::make_shared<Material>(col1, vertexShaders[0], pixelShaders[1]));
	mats.push_back(std::make_shared<Material>(col1, vertexShaders[0], pixelShaders[2]));
	mats.push_back(std::make_shared<Material>(col1, vertexShaders[0], pixelShaders[3]));

	XMFLOAT4 col2 = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	mats.push_back(std::make_shared<Material>(col2, vertexShaders[0], pixelShaders[0]));
	mats.push_back(std::make_shared<Material>(col2, vertexShaders[0], pixelShaders[1]));
	mats.push_back(std::make_shared<Material>(col2, vertexShaders[0], pixelShaders[2]));
	mats.push_back(std::make_shared<Material>(col2, vertexShaders[0], pixelShaders[3]));

	//XMFLOAT4 col3 = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	//mats.push_back(std::make_shared<Material>(col3, vertexShader, pixelShader));

}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	//Create meshes
	std::shared_ptr mesh1 = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.obj").c_str());
	entities.push_back(std::make_shared<GameEntity>(mesh1, mats[0]));
	entities.push_back(std::make_shared<GameEntity>(mesh1, mats[1]));
	entities.push_back(std::make_shared<GameEntity>(mesh1, mats[2]));
	entities.push_back(std::make_shared<GameEntity>(mesh1, mats[3]));
	std::shared_ptr mesh2 = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str());
	entities.push_back(std::make_shared<GameEntity>(mesh2, mats[4]));
	entities.push_back(std::make_shared<GameEntity>(mesh2, mats[5]));
	entities.push_back(std::make_shared<GameEntity>(mesh2, mats[6]));
	entities.push_back(std::make_shared<GameEntity>(mesh2, mats[7]));

}

void Game::ImguiUpdateHelper(float deltaTime) {
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
}

/// <summary>
/// Creates the GUI
/// </summary>
void Game::BuildUI() {
	ImGui::Begin("My GUI");

	ImGui::Text("Frame Rate: %f fps", ImGui::GetIO().Framerate);
	ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());

	//BG color picker
	ImGui::ColorEdit4("Background Color", bgColor);

	//if (ImGui::CollapsingHeader("Mesh Data")) {
	//	for (int i = 0; i < meshes.size(); i++) {
	//		ImGui::Text("Mesh #%d:", i + 1);
	//		ImGui::Text("\tTriangles: %d", meshes.at(i)->getIndexCount() / 3);
	//		ImGui::Text("\tVertices: %d", meshes.at(i)->getVertexCount());
	//		ImGui::Text("\tIndices: %d", meshes.at(i)->getIndexCount());
	//	}
	//}
	//
	//if (ImGui::CollapsingHeader("Constant Buffer")) {
	//	ImGui::SliderFloat3("Offset", &vsData.offset.x, -1, 1);
	//	ImGui::ColorEdit4("Color", &vsData.color.x);
	//}
	{
		ImGui::Text("Current Camera Info:");
		ImGui::PushID(0);
		XMFLOAT3 pos = cams[activeCam]->GetTransform()->GetPosition();
		ImGui::Text("Position: %f,%f,%f", pos.x, pos.y, pos.z);
		XMFLOAT3 rot = cams[activeCam]->GetTransform()->GetPitchYawRoll();
		ImGui::Text("Rotation: %f,%f,%f", rot.x, rot.y, rot.z);
		ImGui::Text("FOV: %f", XMConvertToDegrees(cams[activeCam]->getFov()));

		if (ImGui::RadioButton("Cam 1", activeCam == 0)) {
			activeCam = 0;
		}
		if (ImGui::RadioButton("Cam 2", activeCam == 1)) {
			activeCam = 1;
		}
	}

	ImGui::PopID();

	/*for (int i = 0; i < entities.size(); i++) {
		ImGui::Text("Mesh #%d:", i+1);
		ImGui::PushID(i+1);

		entities[i]->GetTransform()->SetPosition(entityTransformValues[3 * i]);
		ImGui::DragFloat3("Position", &entityTransformValues[3 * i].x, 0.01f);

		entities[i]->GetTransform()->SetRotation(entityTransformValues[3 * i + 1]);
		ImGui::DragFloat3("Rotation", &entityTransformValues[3 * i + 1].x, 0.01f);

		entities[i]->GetTransform()->SetScale(entityTransformValues[3 * i + 2]);
		ImGui::DragFloat3("Scale", &entityTransformValues[3 * i + 2].x, 0.01f);

		ImGui::PopID();
	}*/

	ImGui::End();
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (auto cam : cams) {
		if (cam != nullptr) {
			cam->UpdateProjectionMatrix(Window::AspectRatio());
		}
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	ImguiUpdateHelper(deltaTime);
	BuildUI();
	//move objects;
	//entities[2]->GetTransform()->SetPosition(sin(totalTime), 0, 0);
	//entities[4]->GetTransform()->SetRotation(0, 0, totalTime);
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->GetTransform()->SetPosition((float) i * 3 - 10.5f, 0, 0);
	}
	cams[activeCam]->Update(deltaTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	bgColor);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		//Set up camera
		vsData.view = cams[activeCam]->getViewMatrix();
		vsData.projection = cams[activeCam]->getProjectionMatrix();
	
		vsData.time = totalTime;
	}	

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		for (std::shared_ptr<GameEntity> i : entities) {
			vsData.world = i->GetTransform()->GetWorldMatrix();
			D3D11_MAPPED_SUBRESOURCE mappedBufferV = {};
			Graphics::Context->Map(vertexConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBufferV);
			memcpy(mappedBufferV.pData, &vsData, sizeof(vsData));
			Graphics::Context->Unmap(vertexConstantBuffer.Get(), 0);
			Graphics::Context->VSSetConstantBuffers(0, 1, vertexConstantBuffer.GetAddressOf());

			psData.colorTint = i->GetMat()->GetColorTint();
			D3D11_MAPPED_SUBRESOURCE mappedBufferP = {};
			Graphics::Context->Map(pixelConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBufferP);
			memcpy(mappedBufferP.pData, &psData, sizeof(psData));
			Graphics::Context->Unmap(pixelConstantBuffer.Get(), 0);
			Graphics::Context->PSSetConstantBuffers(0, 1, pixelConstantBuffer.GetAddressOf());
		
			i->Draw();
		}
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		//Create the UI
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



