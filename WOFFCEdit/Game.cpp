//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "DisplayObject.h"
#include <string>


using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()

{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
	m_displayList.clear();
	
	//initial Settings
	//modes
	m_grid = false;

	//functional
	m_movespeed = 0.10;
	m_camRotRate = 3.0;

	rayOrigin = Vector3(0.0, 0.0, 0.0);
	rayDirection = Vector3(0.0, 0.0, 0.0);

	buttonCheck = false;
}

Game::~Game()
{

#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);
	m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
	

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	GetClientRect(window, &m_ScreenDimensions);


#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

void Game::SetGridState(bool state)
{
	m_grid = state;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick(InputCommands *Input)
{
	//copy over the input commands so we have a local version to use elsewhere.
	m_InputCommands = *Input;
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });


#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

    Render();
}

void Game::InputHandler()
{
	//takes about a second to register
	//ShowCursor(m_InputCommands.showCursor);

	//process mouse 
	if (m_InputCommands.rightMouseDown)
	{
		camera.mouseRot(m_InputCommands.mouseDragX*50, -m_InputCommands.mouseDragY*50);
	}
	else
	{
	}
	if(m_InputCommands.terrainPainting )
	{
		if (m_InputCommands.scaleUp && !buttonCheck)
		{
			buttonCheck = true;
			m_displayChunk.strength += 0.1;
		}
		if (m_InputCommands.scaleDown && !buttonCheck)
		{
			buttonCheck = true;
			m_displayChunk.strength -= 0.1;
		}
		else if(!m_InputCommands.buttonDown)
		{
			buttonCheck = false;
		}
	}

	switch (objectSelected)
	{
	case true:
		//process input and update stuff
		if (!m_InputCommands.rotateMode && !m_InputCommands.scaleMode)
		{
			if (m_InputCommands.forward)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_position.x += 0.1;
			}
			if (m_InputCommands.back)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_position.x -= 0.1;
			}
			if (m_InputCommands.right)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_position.z += 0.1;
			}
			if (m_InputCommands.left)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_position.z -= 0.1;
			}
			if (m_InputCommands.up)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_position.y += 0.1;
			}
			if (m_InputCommands.down)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_position.y -= 0.1;
			}
		}
		if(m_InputCommands.rotateMode)
		{
			
			if (m_InputCommands.forward)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_orientation.x += 1.0;
			}
			if (m_InputCommands.back)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_orientation.x -= 1.0;
			}
			if (m_InputCommands.right)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_orientation.z += 1.0;
			}
			if (m_InputCommands.left)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_orientation.z -= 1.0;
			}

		}
		if(m_InputCommands.scaleMode)
		{
			if (m_InputCommands.scaleUp)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_scale += Vector3(0.1, 0.1, 0.1);
			}
			if (m_InputCommands.scaleDown)
			{
				if (selectedID != -1)
					m_displayList[selectedID].m_scale -= Vector3(0.1, 0.1, 0.1);
			}
		}
		break;
	case false:
		//process input and update stuff
		if (m_InputCommands.forward)
		{
			camera.moveForward();
		}
		if (m_InputCommands.back)
		{
			camera.moveBack();
		}
		if (m_InputCommands.right)
		{
			camera.StrafeRight();
		}
		if (m_InputCommands.left)
		{
			camera.StrafeLeft();
		}
		if (m_InputCommands.up)
		{
			camera.moveUp();
		}
		if (m_InputCommands.down)
		{
			camera.moveDown();
		}
		if(m_InputCommands.rotLeft)
		{
			camera.rotLeft();
		}
		if (m_InputCommands.rotRight)
		{
			camera.rotRight();
		}
	}

	recordActions();
	//undoLastAction();
	//objectheightChek();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	mouseLoc.x = m_InputCommands.mouse_X;
	mouseLoc.y = m_InputCommands.mouse_Y;

	InputHandler();
	camera.update();
	m_view = Matrix::CreateLookAt(camera.getPosition(), camera.getLookAt(), camera.getUp());

	m_batchEffect->SetView(m_view);
	m_batchEffect->SetWorld(Matrix::Identity);
	m_displayChunk.m_terrainEffect->SetView(m_view);
	m_displayChunk.m_terrainEffect->SetWorld(Matrix::Identity);

#ifdef DXTK_AUDIO
	m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
	if (m_audioTimerAcc < 0)
	{
		if (m_retryDefault)
		{
			m_retryDefault = false;
			if (m_audEngine->Reset())
			{
				// Restart looping audio
				m_effect1->Play(true);
			}
		}
		else
		{
			m_audioTimerAcc = 4.f;

			m_waveBank->Play(m_audioEvent++);

			if (m_audioEvent >= 11)
				m_audioEvent = 0;
		}
	}
#endif
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }
	Clear();
	//int MousePicking();
    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

	if (m_grid)
	{
		// Draw procedurally generated dynamic grid
		const XMVECTORF32 xaxis = { 512.f, 0.f, 0.f };
		const XMVECTORF32 yaxis = { 0.f, 0.f, 512.f };
		DrawGrid(xaxis, yaxis, g_XMZero, 512, 512, Colors::Gray);
	}

	//RENDER OBJECTS FROM SCENEGRAPH
	int numRenderObjects = m_displayList.size();
	for (int i = 0; i < numRenderObjects; i++)
	{
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x, m_displayList[i].m_scale.y, m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x, m_displayList[i].m_position.y, m_displayList[i].m_position.z };

		//convert degrees into radians for rotation matrix
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y *3.1415 / 180,
															m_displayList[i].m_orientation.x *3.1415 / 180,
															m_displayList[i].m_orientation.z *3.1415 / 180);

		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		m_displayList[i].m_model->Draw(context, *m_states, local, m_view, m_projection, false);	//last variable in draw,  make TRUE for wire frame

		m_deviceResources->PIXEndEvent();
	}
    m_deviceResources->PIXEndEvent();

	//RENDER TERRAIN
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(),0);
	context->RSSetState(m_states->CullNone());
	if (m_InputCommands.wireFrame)
	{
		context->RSSetState(m_states->Wireframe()); //uncomment for wireframe
	}
	//Render the batch,  This is handled in the Display chunk because it has the potential to get complex
	m_displayChunk.RenderBatch(m_deviceResources);

	//Draw HUD
	m_sprites->Begin();
	WCHAR   Buffer[256];
	//std::wstring var = L"World X: " + std::to_wstring(m_InputCommands.Undo) + L"World Y: " + std::to_wstring(m_displayChunk.pick.y) + L"World Z: " + std::to_wstring(m_displayChunk.pick.z);

	if(m_InputCommands.terrainPainting)
	{
		std::wstring var = L"Current Mode: Terrain Painting";
		m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Blue);
		std::wstring var2 = L"Paint Strength: " + std::to_wstring(m_displayChunk.strength);
		m_font->DrawString(m_sprites.get(), var2.c_str(), XMFLOAT2(100, 40), Colors::Blue);
	}
	else if(m_InputCommands.rotateMode && selectedID != -1)
	{
		std::wstring var = L"Current Mode: Object Selected in Rotate Mode";
		m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Blue);
	}
	else if(m_InputCommands.rotateMode)
	{
		std::wstring var = L"Current Mode: Rotate";
		m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Blue);
	}
	else if(m_InputCommands.scaleMode && selectedID != -1)
	{
		std::wstring var = L"Current Mode: Object Selected in Scale Mode";
		m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Blue);
	}
	else if(m_InputCommands.scaleMode)
	{
		std::wstring var = L"Current Mode: Scale";
		m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Blue);
	}
	else if(selectedID != -1)
	{
		std::wstring var = L"Current Mode: Object Selected in Free Mode";
		m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Blue);
	}
	else
	{
		std::wstring var = L"Current Mode: Free Mode";
		m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Blue);
	}
	m_sprites->End();

    m_deviceResources->Present();
}

int Game::MousePicking()
{
	float pickedDistance = 0;
	selectedID = -1;
	int curClosestID = -1;
	if (selectedID = -1)
	{
		objectSelected = false;
	}
	//setup near and far planes of frustum with mouse X and mouse y passed down from Toolmain. 
		//they may look the same but note, the difference in Z
	const XMVECTOR nearSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 0.0f, 1.0f);
	const XMVECTOR farSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 1.0f, 1.0f);
	//Loop through entire display list of objects and pick with each in turn. 
	for (int i = 0; i < m_displayList.size(); i++)
	{
		//Get the scale factor and translation of the object
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x,		m_displayList[i].m_scale.y,		m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x,		m_displayList[i].m_position.y,	m_displayList[i].m_position.z };
		//convert euler angles into a quaternion for the rotation of the object
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y *3.1415 / 180,
															m_displayList[i].m_orientation.x *3.1415 / 180,
															m_displayList[i].m_orientation.z *3.1415 / 180);
		//create set the matrix of the selected object in the world based on the translation, scale and rotation.
		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);
		//Unproject the points on the near and far plane, with respect to the matrix we just created.
		XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);
		XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);
		//turn the transformed points into our picking vector. 
		XMVECTOR pickingVector = farPoint - nearPoint;
		//pickedPosition = pickingVector;
		pickingVector = XMVector3Normalize(pickingVector);
		//loop through mesh list for object
		for (int y = 0; y < m_displayList[i].m_model.get()->meshes.size(); y++)
		{
			//checking for ray intersection
			if (m_displayList[i].m_model.get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance))
			{
				selectedID = i;
				lastSelectedID = i;
				objectSelected = true;
			}
		}
	}
	//if we got a hit.  return it.  
	return selectedID;
}

void Game::recordActions()
{
	if(selectedID != -1)
	{
		if(m_InputCommands.buttonDown && !buttonCheck)
		{
			buttonCheck = true;

			undoState.ID = selectedID;
			undoState.scale = m_displayList[selectedID].m_scale;
			undoState.rot = m_displayList[selectedID].m_orientation;
			undoState.pos = m_displayList[selectedID].m_position;

			testUndo.push_back(undoState);
		}
		if(!m_InputCommands.buttonDown)
		{
			buttonCheck = false;
		}
	}
}

void Game::undoLastAction()
{
	if (testUndo.size() >= 1)
	{
		if(testRedo.size() <= 0)
		{
			redoState.ID = lastSelectedID;
			redoState.scale = m_displayList[lastSelectedID].m_scale;
			redoState.rot = m_displayList[lastSelectedID].m_orientation;
			redoState.pos = m_displayList[lastSelectedID].m_position;
			testRedo.push_back(redoState);
		}
		else
		{
			redoState.ID    = testUndo.back().ID;
			redoState.scale = testUndo.back().scale;
			redoState.rot   = testUndo.back().rot;;
			redoState.pos   = testUndo.back().pos;
			testRedo.push_back(redoState);
		}
		//undo from the old data
		int itemID                          = testUndo.back().ID;
		m_displayList[itemID].m_scale       = testUndo.back().scale;
		m_displayList[itemID].m_orientation = testUndo.back().rot;
		m_displayList[itemID].m_position    = testUndo.back().pos;
		testUndo.pop_back();
		//pop from the back of the undo stack
	}
}

void Game::redoLastAction()
{
	if (testRedo.size() >= 1)
	{
		if(testUndo.size() <= 0)
		{
			lastSelectedID = testRedo.back().ID;
			undoState.ID = lastSelectedID;
			undoState.scale = m_displayList[lastSelectedID].m_scale;
			undoState.rot = m_displayList[lastSelectedID].m_orientation;
			undoState.pos = m_displayList[lastSelectedID].m_position;
			testUndo.push_back(undoState);
		}
		//if (testUndo.size() >= 1)
		else
		{
			undoState.ID = testUndo.back().ID;
			undoState.scale = testUndo.back().scale;
			undoState.rot = testUndo.back().rot;;
			undoState.pos = testUndo.back().pos;
			testUndo.push_back(undoState);
		}
		//undo from the old data
		int itemID = testRedo.back().ID;
		m_displayList[itemID].m_scale = testRedo.back().scale;
		m_displayList[itemID].m_orientation = testRedo.back().rot;
		m_displayList[itemID].m_position = testRedo.back().pos;

		//pop from the back of the undo stack
		testRedo.pop_back();
	}
}

void Game::TerrainSculptingCheck()
 {
 	//float pickedDistance = 0;
 
 	//setup near and far planes of frustum with mouse X and mouse y passed down from Toolmain. 
 	const XMVECTOR nearSource = XMVectorSet(mouseLoc.x, mouseLoc.y, 0.0f, 1.0f);
 	const XMVECTOR farSource = XMVectorSet(mouseLoc.x, mouseLoc.y, 1.0f, 1.0f);
 
 	XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection,m_view,m_world);
 	XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, m_world);
 	//turn the transformed points into our picking vector. 
 	XMVECTOR pickingVector = farPoint - nearPoint;

 	//pickingVector = XMVector3Normalize(pickingVector);

	rayOrigin = nearPoint;
	rayDirection = pickingVector;

	m_displayChunk.UpdateTerrain(rayOrigin, rayDirection);
 }

 void Game::objectheightChek()
 {
	 //float pointX, pointY;
	 XMMATRIX inverseWorldMatrix;
	 Vector3 direction, origin;




	 for (int i = 0; i < m_displayList.size(); i++)
	 {
		 origin = m_displayList[1].m_position;
		 const XMVECTOR nearSource = XMVectorSet(origin.x, 0.0f, origin.z, 1.0f);
		 const XMVECTOR farSource = XMVectorSet(origin.x, 1.0f, origin.z, 1.0f);
		 direction = farSource - nearSource;
		 Vector3 col = m_displayChunk.updateObjectHeight(origin, direction);
		 m_displayList[i].m_position.y = col.y+10;
	 }
 }

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
    m_deviceResources->PIXBeginEvent(L"Draw grid");

    auto context = m_deviceResources->GetD3DDeviceContext();
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullCounterClockwise());

    m_batchEffect->Apply(context);

    context->IASetInputLayout(m_batchInputLayout.Get());

    m_batch->Begin();

    xdivs = std::max<size_t>(1, xdivs);
    ydivs = std::max<size_t>(1, ydivs);

    for (size_t i = 0; i <= xdivs; ++i)
    {
        float fPercent = float(i) / float(xdivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    for (size_t i = 0; i <= ydivs; i++)
    {
        float fPercent = float(i) / float(ydivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    m_batch->End();

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void Game::BuildDisplayList(std::vector<SceneObject> * SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	if (!m_displayList.empty())		//is the vector empty
	{
		m_displayList.clear();		//if not, empty it
	}

	//for every item in the scenegraph
	int numObjects = SceneGraph->size();
	for (int i = 0; i < numObjects; i++)
	{
		
		//create a temp display object that we will populate then append to the display list.
		DisplayObject newDisplayObject;
		
		//load model
		std::wstring modelwstr = StringToWCHART(SceneGraph->at(i).model_path);							//convect string to Wchar
		newDisplayObject.m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		std::wstring texturewstr = StringToWCHART(SceneGraph->at(i).tex_diffuse_path);								//convect string to Wchar
		HRESULT rs;
		rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
		}

		//apply new texture to models effect
		newDisplayObject.m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{	
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);			
			}
		});

		//set position
		newDisplayObject.m_position.x = SceneGraph->at(i).posX;
		newDisplayObject.m_position.y = SceneGraph->at(i).posY;
		newDisplayObject.m_position.z = SceneGraph->at(i).posZ;
		
		//setorientation
		newDisplayObject.m_orientation.x = SceneGraph->at(i).rotX;
		newDisplayObject.m_orientation.y = SceneGraph->at(i).rotY;
		newDisplayObject.m_orientation.z = SceneGraph->at(i).rotZ;

		//set scale
		newDisplayObject.m_scale.x = SceneGraph->at(i).scaX;
		newDisplayObject.m_scale.y = SceneGraph->at(i).scaY;
		newDisplayObject.m_scale.z = SceneGraph->at(i).scaZ;

		//set wireframe / render flags
		newDisplayObject.m_render		= SceneGraph->at(i).editor_render;
		newDisplayObject.m_wireframe	= SceneGraph->at(i).editor_wireframe;

		newDisplayObject.m_light_type		= SceneGraph->at(i).light_type;
		newDisplayObject.m_light_diffuse_r	= SceneGraph->at(i).light_diffuse_r;
		newDisplayObject.m_light_diffuse_g	= SceneGraph->at(i).light_diffuse_g;
		newDisplayObject.m_light_diffuse_b	= SceneGraph->at(i).light_diffuse_b;
		newDisplayObject.m_light_specular_r = SceneGraph->at(i).light_specular_r;
		newDisplayObject.m_light_specular_g = SceneGraph->at(i).light_specular_g;
		newDisplayObject.m_light_specular_b = SceneGraph->at(i).light_specular_b;
		newDisplayObject.m_light_spot_cutoff = SceneGraph->at(i).light_spot_cutoff;
		newDisplayObject.m_light_constant	= SceneGraph->at(i).light_constant;
		newDisplayObject.m_light_linear		= SceneGraph->at(i).light_linear;
		newDisplayObject.m_light_quadratic	= SceneGraph->at(i).light_quadratic;
		
		m_displayList.push_back(newDisplayObject);
	}
}

void Game::BuildDisplayChunk(ChunkObject * SceneChunk)
{
	//populate our local DISPLAYCHUNK with all the chunk info we need from the object stored in toolmain
	//which, to be honest, is almost all of it. Its mostly rendering related info so...
	m_displayChunk.PopulateChunkData(SceneChunk);		//migrate chunk data
	m_displayChunk.LoadHeightMap(m_deviceResources);
	m_displayChunk.m_terrainEffect->SetProjection(m_projection);
	m_displayChunk.InitialiseBatch();
}

void Game::SaveDisplayChunk(ChunkObject * SceneChunk)
{
	m_displayChunk.SaveHeightMap();			//save heightmap to file.
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif


#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);

    m_fxFactory = std::make_unique<EffectFactory>(device);
	m_fxFactory->SetDirectory(L"database/data/"); //fx Factory will look in the database directory
	m_fxFactory->SetSharing(false);	//we must set this to false otherwise it will share effects based on the initial tex loaded (When the model loads) rather than what we will change them to.

    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

    //m_shape = GeometricPrimitive::CreateTeapot(context, 4.f, 8);

    // SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
    m_model = Model::CreateFromSDKMESH(device, L"tiny.sdkmesh", *m_fxFactory);
	

    // Load textures
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf())
    );

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"windowslogo.dds", nullptr, m_texture2.ReleaseAndGetAddressOf())
    );

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    m_batchEffect->SetProjection(m_projection);
	
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batch.reset();
    m_batchEffect.reset();
    m_font.reset();
    m_shape.reset();
    m_model.reset();
    m_texture1.Reset();
    m_texture2.Reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

std::wstring StringToWCHART(std::string s)
{

	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
