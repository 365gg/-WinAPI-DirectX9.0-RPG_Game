#include "GameSystem.h"
#include <Windows.h>
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam) {
	switch (msg)
	{
	case WM_KEYDOWN:
		if (VK_ESCAPE == wParam)
			DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDOWN:
		MessageBox(0, L"Hello World", L"Hello", MB_OK);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lparam);
}
GameSystem* GameSystem::_instance = NULL;
GameSystem::GameSystem()
{
	_isEnable4xMsaa = false;
}
GameSystem::~GameSystem()
{
	_swapChain->Release();
	_swapChain = 0;

	_d3dDevice->Release();
	_d3dDevice = 0;

	_d3dDeviceContext->Release();
	_d3dDeviceContext = 0;

	_renderTargetView->Release();
	_renderTargetView = 0;

	_depthStencilBuffer->Release();
	_depthStencilBuffer = 0;

	_depthStencilView->Release();
	_depthStencilView = 0;
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
	_hMainWnd = CreateWindow(L"Base", L"2D_RPG_TileMap", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, 0);
	//윈도우 출력
	ShowWindow(_hMainWnd, nCmdShow);
	//윈도우 업데이트
	UpdateWindow(_hMainWnd);
	if (false == InitDirect3D())
	{
		return false;
	}
	return true;
}
int GameSystem::Update()
{
	MSG msg = { 0 };
	int ref = 1;
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

			float color[4];
			color[0] = 0.0f;	//R
			color[1] = 0.2f;	//G
			color[2] = 0.3f;	//B
			color[3] = 1.0f;	//Alpha

			_d3dDeviceContext->ClearRenderTargetView(_renderTargetView,color);
			_d3dDeviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			//게임 관련 드로우
			_swapChain->Present(0, 0);
		}
	}
	return (int)msg.wParam;
}
bool GameSystem::InitDirect3D()
{
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(0,D3D_DRIVER_TYPE_HARDWARE,0,D3D10_CREATE_DEVICE_DEBUG,0,0,D3D11_SDK_VERSION,&_d3dDevice,&featureLevel,&_d3dDeviceContext);
	hr = _d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4xMsaaQuality);	//4xMSAA 지원여부 확인
	if (FAILED(hr))
	{
		MessageBox(0, L"4xMSAA 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	//클라이언트 화면 크기의 특징 서술
	DXGI_SWAP_CHAIN_DESC swapChain;
	swapChain.BufferDesc.Width = 1280;
	swapChain.BufferDesc.Height = 800;
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (_isEnable4xMsaa)
	{
		swapChain.SampleDesc.Count = 4;
		swapChain.SampleDesc.Quality = _4xMsaaQuality-1;
	}
	else
	{
		swapChain.SampleDesc.Count = 1;
		swapChain.SampleDesc.Quality = 0;
	}
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 1;
	swapChain.OutputWindow = _hMainWnd;
	swapChain.Windowed = true;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Flags = 0;

	// IDXGISwapChain 인스턴스 생성(하기 위해서 _dxgiFactory를 가져옴)
	IDXGIDevice* dxgiDevice = 0;
	hr = _d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr))
	{
		MessageBox(0, L"dxgiDevice 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	IDXGIAdapter* dxgiAdapter = 0;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	if (FAILED(hr))
	{
		MessageBox(0, L"dxgiAdapter 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	IDXGIFactory* dxgiFactory = 0;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hr))
	{
		MessageBox(0, L"dxgiFactory 에러입니다.", L"ErrorMessage", 0);
		return false;
	}

	hr = dxgiFactory->CreateSwapChain(_d3dDevice,&swapChain,&_swapChain);
	if (FAILED(hr))
	{
		MessageBox(0, L"dxgiFactory 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	//메모리 해제
	dxgiDevice->Release();
	dxgiDevice = 0;

	dxgiAdapter->Release();
	dxgiAdapter = 0;

	dxgiFactory->Release();
	dxgiFactory = 0;

	//SwapChain 후면 버퍼 대한 랜더 대상 뷰 생성
	ID3D11Texture2D* backBuffer;
	hr = _swapChain->GetBuffer(
		0,	//백버퍼 인덱스
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
	{
		MessageBox(0, L"backBuffer 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	hr = _d3dDevice->CreateRenderTargetView(backBuffer, 0, &_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(0, L"_renderTargetView 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	backBuffer->Release();
	backBuffer = 0;
	//깊이 스텐실 버퍼와 그에 연결되는 깊이 스텐실 뷰 생성
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = 1280;
	depthStencilDesc.Height = 800;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (_isEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = _4xMsaaQuality;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	hr = _d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &_depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"_depthStencilBuffer 에러입니다.", L"ErrorMessage", 0);
		return false;
	}

	hr = _d3dDevice->CreateDepthStencilView(_depthStencilBuffer, 0, &_depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(0, L"_depthStencilView 에러입니다.", L"ErrorMessage", 0);
		return false;
	}
	// 렌더 대상 뷰와 깊이 스텐실 뷰를 Direct3D가 사용할 수 있도록 렌더링 파이프리인의 출력 병합기에 묶는다.
	_d3dDeviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// 뷰포트 설정
	_screenViewport.TopLeftX = 0;
	_screenViewport.TopLeftY = 0;
	_screenViewport.Width = 1280.0f;
	_screenViewport.Height = 800.0f;
	_screenViewport.MinDepth = 0.0f;
	_screenViewport.MaxDepth = 1.0f;

	_d3dDeviceContext->RSSetViewports(1, &_screenViewport);
	return true;
}