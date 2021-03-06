#include "GameSystem.h"
#include <Windows.h>
#include "GameTimer.h"
#include "ComponentSystem.h"
#include "RecoveryItem.h"
#include "Stage.h"
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam) {
	int mouseX;
	int mouseY;
	switch (msg)
	{
	case WM_KEYDOWN:
		GameSystem::GetInstance()->KeyDown(wParam);
		if (VK_ESCAPE == wParam)
		{
			ComponentSystem::GetInstance()->RemoveAllComponents();
			DestroyWindow(hWnd);
		}
		return 0;
	case WM_KEYUP:
		GameSystem::GetInstance()->KeyUp(wParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDOWN:
		mouseX = LOWORD(lparam);
		mouseY = HIWORD(lparam);
		GameSystem::GetInstance()->MouseDown(mouseX, mouseY);
		return 0;
	case WM_LBUTTONUP:
		GameSystem::GetInstance()->MouseUp();
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lparam);
}
GameSystem* GameSystem::_instance = NULL;
GameSystem::GameSystem()
{
	_isFullScreen = false;
	if (_isFullScreen)
	{
		_WindowWidth = 1920;
		_WindowHeight = 1080;
	}
	else
	{
		_WindowWidth = 1280;
		_WindowHeight = 800;
	}
}
GameSystem::~GameSystem()
{
	_stage->DInit();
	delete _stage;

	RELEASE_COM(_sprite);
	RELEASE_COM(_device3d);
}
GameSystem* GameSystem::GetInstance()
{
	if (NULL == _instance)
		_instance = new GameSystem();
	return _instance;
}
bool GameSystem::InitSystem(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Base";
	//윈도우 등록
	if (!RegisterClass(&wc))
	{
		return false;
	}
	//윈도우 생성
	DWORD style;
	if (_isFullScreen)
	{
		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
	}
	else
	{
		style = WS_OVERLAPPEDWINDOW;
	}
	_hMainWnd = CreateWindow(
		L"Base",
		L"2D_RPG_TileMap",
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		_WindowWidth,
		_WindowHeight,
		0,
		0,
		hInstance,
		0);

	if (false == _isFullScreen)
	{
		RECT clientRect;
		GetClientRect(_hMainWnd, &clientRect);

		int addWidth = _WindowWidth - clientRect.right;
		int addHeight = _WindowHeight - clientRect.bottom;
		int finalWidth = _WindowWidth + addWidth;
		int finalHeight = _WindowHeight + addHeight;
		MoveWindow(_hMainWnd, 0, 0, finalWidth, finalHeight, TRUE);
	}
	//윈도우 출력
	ShowWindow(_hMainWnd, nCmdShow);
	//윈도우 업데이트
	UpdateWindow(_hMainWnd);
	if (false == InitDirect3D())
	{
		return false;
	}

	InitInput();
	_stage = new Stage();
	_stage->Init(L"Map1");
	return true;
}
int GameSystem::Update()
{
	MSG msg = { 0 };
	int ref = 1;
	GameTimer* _gameTimer = new GameTimer();
	_gameTimer->Reset();
	float frameTime = 1.0f / 60.f;	//60프레임
	float frameDuration = 0;
	float colorRed = 0.0f;
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//게임업데이트
			_gameTimer->Update();
			float deltaTime = _gameTimer->GetDeltaTime();
			frameDuration += deltaTime;

			ComponentSystem::GetInstance()->Update(deltaTime);

			_stage->Update(deltaTime);
			if (frameTime <= frameDuration)
			{

				frameDuration = 0.0f;

				_device3d->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 100, 0), 0.0f, 0);

				_device3d->BeginScene();

				_sprite->Begin(D3DXSPRITE_ALPHABLEND);

				// sprite test
				_stage->Render();
								
				_sprite->End();

				_device3d->EndScene();

				CheckDeviceLost();

				_device3d->Present(NULL, NULL, NULL, NULL);
			}
			//stage change test
			{
				if (IsKeyDown(VK_F1))
				{
					ComponentSystem::GetInstance()->ClearMessageQueue();
					_stage->DInit();
					delete _stage;
					_stage = new Stage();
					_stage->Init(L"Map2");
				}
				if (IsKeyDown(VK_F2))
				{
					ComponentSystem::GetInstance()->ClearMessageQueue();
					_stage->DInit();
					delete _stage;
					_stage = new Stage();
					_stage->Init(L"Map3");
				}
				if (IsKeyDown(VK_F3))
				{
					ComponentSystem::GetInstance()->ClearMessageQueue();
					_stage->DInit();
					delete _stage;
					_stage = new Stage();
					_stage->Init(L"Map4");
				}
			}

			/*wchar_t timeCheck[256];
			swprintf(timeCheck,L"deltaTime %f\n",deltaTime);
			OutputDebugString(timeCheck);*/
		}
	}
	return (int)msg.wParam;
}
bool GameSystem::InitDirect3D()
{
	LPDIRECT3D9 direct3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (NULL == direct3d)
	{
		MessageBox(0, L"false", L"create Filed", MB_OK);
		return false;
	}

	ZeroMemory(&_d3dpp, sizeof(_d3dpp));
	_d3dpp.BackBufferWidth = _WindowWidth;
	_d3dpp.BackBufferHeight = _WindowHeight;

	if (_isFullScreen)
	{
		_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	}

	_d3dpp.BackBufferCount = 1;
	_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	_d3dpp.hDeviceWindow = _hMainWnd;
	_d3dpp.Windowed = (!_isFullScreen);
	_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	D3DCAPS9 caps;
	HRESULT hr = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	if (FAILED(hr))
	{
		MessageBox(0, L"GetDeviceCaps 에러입니다.", L"ErrorMessage", 0);
		return false;
	}

	DWORD behavior;
	if ((caps.DeviceType & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || (caps.VertexShaderVersion < D3DVS_VERSION(1, 1)))
	{
		behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	else
	{
		behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}

	hr = direct3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _hMainWnd, behavior, &_d3dpp, &_device3d);
	if (FAILED(hr))
	{
		MessageBox(0, L"CreateDevice 에러입니다.", L"ErrorMessage", 0);
		return false;
	}

	hr = D3DXCreateSprite(_device3d, &_sprite);
	if (FAILED(hr))
	{
		MessageBox(0, L"D3DXCreateSprite 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	return true;
}
void GameSystem::CheckDeviceLost()
{
	HRESULT hr = _device3d->TestCooperativeLevel();
	if (FAILED(hr))
	{
		if (D3DERR_DEVICELOST == hr)
		{
			Sleep(100);
			return;
		}
		else if (D3DERR_DEVICENOTRESET == hr)
		{
			_stage->Release();

			hr = _device3d->Reset(&_d3dpp);
			InitDirect3D();
			_stage->Reset();
		}
	}
}
int GameSystem::GetWindowWidth()
{
	return _WindowWidth;
}
int GameSystem::GetWindowHeight()
{
	return _WindowHeight;
}
LPD3DXSPRITE GameSystem::GetSprite()
{
	return _sprite;
}
LPDIRECT3DDEVICE9 GameSystem::GetDevice3d()
{
	return _device3d;
}
void GameSystem::MouseDown(int mouseX, int mouseY)
{
	_isMouseDown = true;

	_mouseX = mouseX;
	_mouseY = mouseY;
}
void GameSystem::InitInput()
{
	for (int i = 0; i < 256; i++)
	{
		_keyState[i] = eKeyState::KEY_UP;
	}
	_isMouseDown = false;
	_mouseX = 0;
	_mouseY = 0;
}
void GameSystem::KeyDown(unsigned int keyCode)
{
	_keyState[keyCode] = eKeyState::KEY_DOWN;
}
void GameSystem::KeyUp(unsigned int keyCode)
{
	_keyState[keyCode] = eKeyState::KEY_UP;
}
bool GameSystem::IsKeyDown(int keyCode)
{
	if(eKeyState::KEY_DOWN == _keyState[keyCode])
		return true;
	return false;
}